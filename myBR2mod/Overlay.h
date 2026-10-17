#pragma once
#include <Windows.h>
#include <atomic>
#include <cmath>
#include <cstring>
#include <vector>
#include "Config.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")
#include "PodArchive.h"

// Draws our own UI on top of the game's frame.
//
// The game renders with Direct3D 8 (DXVK translates it to Vulkan, but the game only ever talks to the D3D8 interface).
// Rather than hooking the D3D8 vtable, we hook the game's own present wrapper, FUN_0048f820. It runs once per frame on
// the render thread, and it covers both of the game's present paths (device Present, and a secondary swap chain Present).
// We draw right before the original runs, so whatever we draw lands on top of the finished frame.
//
// The device was created without D3DCREATE_MULTITHREADED, so ALL D3D calls must happen inside the hook (render thread).
// Our main thread should only ever flip atomics that the hook reads.
//
// The Windows SDK no longer ships d3d8.h, so the handful of vtable indices and constants we need are declared below.

// FUN_0048f820. Takes no arguments, returns 1.
typedef int(__cdecl* FN_PresentFrame)();

namespace D3D8 {
	// IDirect3DDevice8 vtable indices
	enum DeviceVtable {
		CreateTexture = 20,
		BeginScene = 34,
		EndScene = 35,
		GetViewport = 41,
		SetRenderState = 50,
		ApplyStateBlock = 54,
		DeleteStateBlock = 56,
		CreateStateBlock = 57,
		SetTexture = 61,
		SetTextureStageState = 63,
		DrawPrimitiveUP = 72,
		SetVertexShader = 76,
		SetPixelShader = 88
	};

	// IDirect3DTexture8 vtable indices
	enum TextureVtable {
		LockRect = 16,
		UnlockRect = 17
	};

	// D3DRENDERSTATETYPE
	constexpr DWORD RS_ZENABLE = 7;
	constexpr DWORD RS_FILLMODE = 8;
	constexpr DWORD RS_ZWRITEENABLE = 14;
	constexpr DWORD RS_ALPHATESTENABLE = 15;
	constexpr DWORD RS_SRCBLEND = 19;
	constexpr DWORD RS_DESTBLEND = 20;
	constexpr DWORD RS_CULLMODE = 22;
	constexpr DWORD RS_ALPHABLENDENABLE = 27;
	constexpr DWORD RS_FOGENABLE = 28;
	constexpr DWORD RS_STENCILENABLE = 52;
	constexpr DWORD RS_LIGHTING = 137;
	constexpr DWORD RS_COLORWRITEENABLE = 168;
	constexpr DWORD RS_BLENDOP = 171;

	// D3DTEXTURESTAGESTATETYPE (D3D8 numbering; sampler states also live here in D3D8, unlike D3D9)
	constexpr DWORD TSS_COLOROP = 1;
	constexpr DWORD TSS_COLORARG1 = 2;
	constexpr DWORD TSS_COLORARG2 = 3;
	constexpr DWORD TSS_ALPHAOP = 4;
	constexpr DWORD TSS_ALPHAARG1 = 5;
	constexpr DWORD TSS_ALPHAARG2 = 6;
	constexpr DWORD TSS_TEXCOORDINDEX = 11;
	constexpr DWORD TSS_ADDRESSU = 13;
	constexpr DWORD TSS_ADDRESSV = 14;
	constexpr DWORD TSS_MAGFILTER = 16;
	constexpr DWORD TSS_MINFILTER = 17;
	constexpr DWORD TSS_MIPFILTER = 18;
	constexpr DWORD TSS_TEXTURETRANSFORMFLAGS = 24;

	// values
	constexpr DWORD FILL_SOLID = 3;
	constexpr DWORD CULL_NONE = 1;
	constexpr DWORD BLEND_SRCALPHA = 5;
	constexpr DWORD BLEND_INVSRCALPHA = 6;
	constexpr DWORD BLENDOP_ADD = 1;
	constexpr DWORD TOP_DISABLE = 1;
	constexpr DWORD TOP_SELECTARG1 = 2;
	constexpr DWORD TOP_MODULATE = 4;
	constexpr DWORD TA_DIFFUSE = 0;
	constexpr DWORD TA_TEXTURE = 2;
	constexpr DWORD TADDRESS_CLAMP = 3;
	constexpr DWORD TEXF_NONE = 0;
	constexpr DWORD TEXF_LINEAR = 2;
	constexpr DWORD SBT_ALL = 1;
	constexpr DWORD PT_TRIANGLESTRIP = 5;
	constexpr DWORD FMT_A8R8G8B8 = 21;
	constexpr DWORD POOL_MANAGED = 1;
	constexpr DWORD FVF_XYZRHW = 0x004;
	constexpr DWORD FVF_DIFFUSE = 0x040;
	constexpr DWORD FVF_TEX1 = 0x100;

	struct Viewport {
		DWORD x;
		DWORD y;
		DWORD width;
		DWORD height;
		float minZ;
		float maxZ;
	};

	struct LockedRect {
		INT pitch;
		void* bits;
	};

	// screen-space (pre-transformed) vertex with a color
	struct ColorVertex {
		float x, y, z, rhw;
		DWORD color; // ARGB
	};
	constexpr DWORD FVF_COLORVERTEX = FVF_XYZRHW | FVF_DIFFUSE;

	// screen-space vertex with a color (multiplied with the texture) and one set of texture coordinates
	struct TexturedVertex {
		float x, y, z, rhw;
		DWORD color; // ARGB
		float u, v;
	};
	constexpr DWORD FVF_TEXTUREDVERTEX = FVF_XYZRHW | FVF_DIFFUSE | FVF_TEX1;

	// COM methods are __stdcall with the interface pointer as the first argument
	template <typename Fn>
	Fn method(void* object, int index) {
		return reinterpret_cast<Fn>((*reinterpret_cast<void***>(object))[index]);
	}
}

// The game's .TEX format: a 0x20 byte header, then raw 32bpp pixels in B, G, R, A byte order,
// which is exactly D3DFMT_A8R8G8B8 in memory, so the pixels can be copied straight into a texture.
struct TexImage {
	static constexpr uint32_t headerSize = 0x20;
	static constexpr uint32_t widthOffset = 0x08;
	static constexpr uint32_t heightOffset = 0x0C;

	std::vector<uint8_t> data;
	uint32_t width = 0;
	uint32_t height = 0;

	bool load(const char* podPath, const char* fileName) {
		if (!PodArchive::readFile(podPath, fileName, this->data)) return false;
		if (this->data.size() < headerSize) return false;

		std::memcpy(&this->width, this->data.data() + widthOffset, sizeof(this->width));
		std::memcpy(&this->height, this->data.data() + heightOffset, sizeof(this->height));

		if (this->data.size() - headerSize != static_cast<size_t>(this->width) * this->height * 4) {
			DEBUG_LOG("TexImage: unexpected size for " << fileName << " (" << this->width << "x" << this->height << ")");
			return false;
		}
		return true;
	}

	const uint8_t* pixels() const {
		return this->data.data() + headerSize;
	}
};

// which weapon mode sits in each ring of SELECTWHEEL_BASE, clockwise from the top. -1 = empty ring.
// the game has six weapon modes, but the wheel has eight rings; the top and bottom ones are left empty.
// shared by the overlay (drawing) and the weapon wheel (stick selection)
namespace WheelLayout {
	constexpr int slotCount = 8;
	constexpr int slotWeapons[slotCount] = { -1, 0, 1, 2, -1, 3, 4, 5 };
	constexpr float slotAngle = 0.785398163f; // 45 degrees, in radians
}

class OverlayHook {
private:
	uintptr_t targetFunctionAddress = 0x0048F820;

	static inline FN_PresentFrame originalFunction = nullptr;

	// image data is loaded on the main thread before the hook is installed, and only read on the render thread
	static inline TexImage wheelBaseImage;
	static inline TexImage wheelIconsImage;

	// D3D textures, created lazily on the render thread. Managed textures survive a device Reset, so we never recreate them.
	// we never release them either; they live as long as the game does.
	static inline void* wheelBaseTexture = nullptr;
	static inline void* wheelIconsTexture = nullptr;
	static inline bool texturesFailed = false;

	// written by the main thread, read on the render thread
	static inline std::atomic<bool> wheelVisible = false;
	static inline std::atomic<uint32_t> unlockedWeapons = 0; // bitmask, same layout as the game's
	static inline std::atomic<int> selectedWeapon = -1; // -1 = nothing selected

	// SELECTWHEEL_BASE.TEX geometry, in pixels of the 256x256 texture.
	// The eight rings sit on a circle around the center, clockwise starting from the top.
	static constexpr float wheelTextureSize = 256.0f;
	static constexpr float ringRadius = 82.0f;
	static constexpr float iconSize = 30.0f; // ring holes are ~26px across, so the icon's black border overlaps the ring a little

	// SELECTWHEEL_ICON.TEX is a 4x4 grid of 32x32 icons. The first six are the weapon modes, in GunKeys::WeaponModes order.
	static constexpr int iconCellSize = 32;
	static constexpr int iconColumns = 4;

	// how big the wheel is drawn, as a fraction of the viewport height
	static constexpr float wheelScreenFraction = 0.6f;
	// the selected icon is drawn this much bigger than the others
	static constexpr float selectedIconScale = 1.3f;

	static constexpr DWORD white = 0xFFFFFFFF;
	static constexpr DWORD unselectedTint = 0xFF909090; // darkens the icons that aren't selected
	static constexpr DWORD backgroundColor = 0x80000000; // dims the game behind the wheel

	bool hookInstalled = false;
	bool hookEnabled = false;

	static int __cdecl hookedPresentFrame() {
		void* device = *Rayne2::D3DDevice;
		if (device != nullptr && wheelVisible.load()) {
			draw(device);
		}
		return originalFunction();
	}

	static void draw(void* device) {
		using namespace D3D8;

		if (!createTextures(device)) return;

		// the game has already ended its scene by the time it presents.
		// if BeginScene fails we are somewhere unexpected, so don't draw this frame
		if (FAILED(method<HRESULT(__stdcall*)(void*)>(device, BeginScene)(device))) return;

		// capture everything the game had set, so we can put it back exactly as we found it.
		// a state block per frame is cheap and means we never hold one across a device Reset
		DWORD stateBlock = 0;
		if (FAILED(method<HRESULT(__stdcall*)(void*, DWORD, DWORD*)>(device, CreateStateBlock)(device, SBT_ALL, &stateBlock))) {
			method<HRESULT(__stdcall*)(void*)>(device, EndScene)(device);
			return;
		}

		setOverlayState(device);

		Viewport viewport = {};
		method<HRESULT(__stdcall*)(void*, Viewport*)>(device, GetViewport)(device, &viewport);
		drawWheel(device, viewport);

		method<HRESULT(__stdcall*)(void*, DWORD)>(device, ApplyStateBlock)(device, stateBlock);
		method<HRESULT(__stdcall*)(void*, DWORD)>(device, DeleteStateBlock)(device, stateBlock);
		method<HRESULT(__stdcall*)(void*)>(device, EndScene)(device);
	}

	static void drawWheel(void* device, const D3D8::Viewport& viewport) {
		float size = viewport.height * wheelScreenFraction;
		float scale = size / wheelTextureSize;
		float centerX = viewport.x + viewport.width * 0.5f;
		float centerY = viewport.y + viewport.height * 0.5f;

		fillRect(device, static_cast<float>(viewport.x), static_cast<float>(viewport.y),
			static_cast<float>(viewport.width), static_cast<float>(viewport.height), backgroundColor);

		drawImage(device, wheelBaseTexture, wheelBaseImage,
			0, 0, wheelBaseImage.width, wheelBaseImage.height,
			centerX - size * 0.5f, centerY - size * 0.5f, size, size, white);

		uint32_t unlocked = unlockedWeapons.load();
		int selected = selectedWeapon.load();
		for (int slot = 0; slot < WheelLayout::slotCount; slot++) {
			int weapon = WheelLayout::slotWeapons[slot];
			// locked weapons leave their ring empty, same as the two rings that never hold a weapon
			if (weapon < 0 || !(unlocked & (1u << weapon))) continue;

			// slot 0 is straight up, going clockwise (screen y points down)
			float angle = slot * WheelLayout::slotAngle;
			float ringX = centerX + sinf(angle) * ringRadius * scale;
			float ringY = centerY - cosf(angle) * ringRadius * scale;

			int iconX = (weapon % iconColumns) * iconCellSize;
			int iconY = (weapon / iconColumns) * iconCellSize;
			bool isSelected = (weapon == selected);
			float iconScreenSize = iconSize * scale * (isSelected ? selectedIconScale : 1.0f);

			drawImage(device, wheelIconsTexture, wheelIconsImage,
				iconX, iconY, iconCellSize, iconCellSize,
				ringX - iconScreenSize * 0.5f, ringY - iconScreenSize * 0.5f, iconScreenSize, iconScreenSize,
				isSelected ? white : unselectedTint);
		}
	}

	static bool createTextures(void* device) {
		if (texturesFailed) return false;
		if (wheelBaseTexture != nullptr && wheelIconsTexture != nullptr) return true;

		wheelBaseTexture = createTexture(device, wheelBaseImage);
		wheelIconsTexture = createTexture(device, wheelIconsImage);

		if (wheelBaseTexture == nullptr || wheelIconsTexture == nullptr) {
			// don't retry every frame
			DEBUG_LOG("Overlay: couldn't create wheel textures");
			texturesFailed = true;
			return false;
		}
		return true;
	}

	static void* createTexture(void* device, const TexImage& image) {
		using namespace D3D8;
		void* texture = nullptr;
		HRESULT result = method<HRESULT(__stdcall*)(void*, UINT, UINT, UINT, DWORD, DWORD, DWORD, void**)>(device, CreateTexture)(
			device, image.width, image.height, 1, 0, FMT_A8R8G8B8, POOL_MANAGED, &texture);
		if (FAILED(result) || texture == nullptr) return nullptr;

		LockedRect locked = {};
		if (FAILED(method<HRESULT(__stdcall*)(void*, UINT, LockedRect*, const RECT*, DWORD)>(texture, LockRect)(texture, 0, &locked, nullptr, 0))) {
			method<ULONG(__stdcall*)(void*)>(texture, 2)(texture); // Release
			return nullptr;
		}

		// the texture's rows may be padded, so copy row by row
		uint32_t rowBytes = image.width * 4;
		for (uint32_t y = 0; y < image.height; y++) {
			std::memcpy(static_cast<uint8_t*>(locked.bits) + y * locked.pitch, image.pixels() + y * rowBytes, rowBytes);
		}

		method<HRESULT(__stdcall*)(void*, UINT)>(texture, UnlockRect)(texture, 0);
		return texture;
	}

	// plain alpha-blended 2D: no depth, no lighting, no fog, no shaders.
	// texture and color/alpha ops are set per draw by drawImage and fillRect
	static void setOverlayState(void* device) {
		using namespace D3D8;
		auto setRenderState = method<HRESULT(__stdcall*)(void*, DWORD, DWORD)>(device, SetRenderState);
		auto setTextureStageState = method<HRESULT(__stdcall*)(void*, DWORD, DWORD, DWORD)>(device, SetTextureStageState);

		setRenderState(device, RS_ZENABLE, FALSE);
		setRenderState(device, RS_ZWRITEENABLE, FALSE);
		setRenderState(device, RS_STENCILENABLE, FALSE);
		setRenderState(device, RS_FILLMODE, FILL_SOLID);
		setRenderState(device, RS_CULLMODE, CULL_NONE);
		setRenderState(device, RS_LIGHTING, FALSE);
		setRenderState(device, RS_FOGENABLE, FALSE);
		setRenderState(device, RS_ALPHATESTENABLE, FALSE);
		setRenderState(device, RS_ALPHABLENDENABLE, TRUE);
		setRenderState(device, RS_BLENDOP, BLENDOP_ADD);
		setRenderState(device, RS_SRCBLEND, BLEND_SRCALPHA);
		setRenderState(device, RS_DESTBLEND, BLEND_INVSRCALPHA);
		setRenderState(device, RS_COLORWRITEENABLE, 0xF);

		setTextureStageState(device, 0, TSS_TEXCOORDINDEX, 0);
		setTextureStageState(device, 0, TSS_TEXTURETRANSFORMFLAGS, 0);
		setTextureStageState(device, 0, TSS_ADDRESSU, TADDRESS_CLAMP);
		setTextureStageState(device, 0, TSS_ADDRESSV, TADDRESS_CLAMP);
		setTextureStageState(device, 0, TSS_MAGFILTER, TEXF_LINEAR);
		setTextureStageState(device, 0, TSS_MINFILTER, TEXF_LINEAR);
		setTextureStageState(device, 0, TSS_MIPFILTER, TEXF_NONE);
		setTextureStageState(device, 1, TSS_COLOROP, TOP_DISABLE);
		setTextureStageState(device, 1, TSS_ALPHAOP, TOP_DISABLE);

		method<HRESULT(__stdcall*)(void*, DWORD)>(device, SetPixelShader)(device, 0);
	}

	static void fillRect(void* device, float x, float y, float width, float height, DWORD color) {
		using namespace D3D8;
		auto setTextureStageState = method<HRESULT(__stdcall*)(void*, DWORD, DWORD, DWORD)>(device, SetTextureStageState);

		method<HRESULT(__stdcall*)(void*, DWORD, void*)>(device, SetTexture)(device, 0, nullptr);
		setTextureStageState(device, 0, TSS_COLOROP, TOP_SELECTARG1);
		setTextureStageState(device, 0, TSS_COLORARG1, TA_DIFFUSE);
		setTextureStageState(device, 0, TSS_ALPHAOP, TOP_SELECTARG1);
		setTextureStageState(device, 0, TSS_ALPHAARG1, TA_DIFFUSE);
		// in D3D8, passing an FVF to SetVertexShader selects the fixed function pipeline
		method<HRESULT(__stdcall*)(void*, DWORD)>(device, SetVertexShader)(device, FVF_COLORVERTEX);

		// -0.5 lines pixel centers up with texel centers in D3D8/9
		float left = x - 0.5f;
		float top = y - 0.5f;
		float right = x + width - 0.5f;
		float bottom = y + height - 0.5f;

		ColorVertex vertices[4] = {
			{ left,  top,    0.0f, 1.0f, color },
			{ right, top,    0.0f, 1.0f, color },
			{ left,  bottom, 0.0f, 1.0f, color },
			{ right, bottom, 0.0f, 1.0f, color }
		};

		method<HRESULT(__stdcall*)(void*, DWORD, UINT, const void*, UINT)>(device, DrawPrimitiveUP)(
			device, PT_TRIANGLESTRIP, 2, vertices, sizeof(ColorVertex));
	}

	// draws the source rectangle (in texture pixels) of a texture into the destination rectangle (in screen pixels).
	// tint is multiplied with the texture, so 0xFFFFFFFF draws it as-is and lowering the alpha fades it
	static void drawImage(void* device, void* texture, const TexImage& image,
		int srcX, int srcY, int srcWidth, int srcHeight,
		float x, float y, float width, float height, DWORD tint) {
		using namespace D3D8;
		auto setTextureStageState = method<HRESULT(__stdcall*)(void*, DWORD, DWORD, DWORD)>(device, SetTextureStageState);

		method<HRESULT(__stdcall*)(void*, DWORD, void*)>(device, SetTexture)(device, 0, texture);
		setTextureStageState(device, 0, TSS_COLOROP, TOP_MODULATE);
		setTextureStageState(device, 0, TSS_COLORARG1, TA_TEXTURE);
		setTextureStageState(device, 0, TSS_COLORARG2, TA_DIFFUSE);
		setTextureStageState(device, 0, TSS_ALPHAOP, TOP_MODULATE);
		setTextureStageState(device, 0, TSS_ALPHAARG1, TA_TEXTURE);
		setTextureStageState(device, 0, TSS_ALPHAARG2, TA_DIFFUSE);
		method<HRESULT(__stdcall*)(void*, DWORD)>(device, SetVertexShader)(device, FVF_TEXTUREDVERTEX);

		float left = x - 0.5f;
		float top = y - 0.5f;
		float right = x + width - 0.5f;
		float bottom = y + height - 0.5f;

		float u0 = static_cast<float>(srcX) / image.width;
		float v0 = static_cast<float>(srcY) / image.height;
		float u1 = static_cast<float>(srcX + srcWidth) / image.width;
		float v1 = static_cast<float>(srcY + srcHeight) / image.height;

		TexturedVertex vertices[4] = {
			{ left,  top,    0.0f, 1.0f, tint, u0, v0 },
			{ right, top,    0.0f, 1.0f, tint, u1, v0 },
			{ left,  bottom, 0.0f, 1.0f, tint, u0, v1 },
			{ right, bottom, 0.0f, 1.0f, tint, u1, v1 }
		};

		method<HRESULT(__stdcall*)(void*, DWORD, UINT, const void*, UINT)>(device, DrawPrimitiveUP)(
			device, PT_TRIANGLESTRIP, 2, vertices, sizeof(TexturedVertex));
	}

public:
	// must be called before install(), so the render thread never sees half-loaded image data
	bool loadImages() {
		if (this->hookInstalled) {
			DEBUG_LOG("Overlay: images must be loaded before the hook is installed");
			return false;
		}
		return wheelBaseImage.load("W32ART.POD", "ART\\SELECTWHEEL_BASE.TEX")
			&& wheelIconsImage.load("W32ART.POD", "ART\\SELECTWHEEL_ICON.TEX");
	}

	bool install() {
		if (this->hookInstalled) {
			DEBUG_LOG("Overlay hook already installed");
			return true;
		}

		MH_STATUS status = MH_Initialize();
		if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
			DEBUG_LOG("Overlay hook failed: " << status);
			return false;
		}

		status = MH_CreateHook(
			(LPVOID)this->targetFunctionAddress,
			(LPVOID)&hookedPresentFrame,
			(LPVOID*)&originalFunction
		);

		if (status != MH_OK) {
			DEBUG_LOG("Overlay CreateHook failed: " << status);
			return false;
		}

		status = MH_EnableHook((LPVOID)this->targetFunctionAddress);
		if (status != MH_OK) {
			DEBUG_LOG("Overlay EnableHook failed: " << status);
			return false;
		}

		this->hookInstalled = true;
		this->hookEnabled = true;
		DEBUG_LOG("Overlay hook installed at 0x" << std::hex << this->targetFunctionAddress);
		return true;
	}

	void uninstall() {
		if (!this->hookInstalled) return;

		// no MH_Uninitialize here: it would tear down every other module's hooks too
		MH_DisableHook((LPVOID)this->targetFunctionAddress);
		MH_RemoveHook((LPVOID)this->targetFunctionAddress);

		this->hookInstalled = false;
		this->hookEnabled = false;
		originalFunction = nullptr;
		DEBUG_LOG("Overlay hook uninstalled");
	}

	void setWheelVisible(bool visible) {
		wheelVisible.store(visible);
	}

	void setUnlockedWeapons(uint32_t bitmask) {
		unlockedWeapons.store(bitmask);
	}

	void setSelectedWeapon(int weapon) {
		selectedWeapon.store(weapon);
	}
};

class Overlay {
private:
	OverlayHook hook;

	// we must wait for the game to create its D3D device before we hook its present function
	bool safeToHook;

public:
	Overlay()
		: safeToHook(false)
	{

	}

	~Overlay() {
		this->hook.uninstall();
	}

	void checkSafeToHook() {
		this->safeToHook = (*Rayne2::D3DDevice != nullptr);
	}

	bool isSafeToHook() {
		return this->safeToHook;
	}

	// loads the game's art the overlay draws with. Must be called before installHook()
	bool loadImages() {
		return this->hook.loadImages();
	}

	bool installHook() {
		if (!this->safeToHook) {
			DEBUG_LOG("Overlay: not safe to hook");
			return false;
		}
		return this->hook.install();
	}

	void showWheel() {
		this->hook.setWheelVisible(true);
	}

	void hideWheel() {
		this->hook.setWheelVisible(false);
	}

	void setUnlockedWeapons(uint32_t bitmask) {
		this->hook.setUnlockedWeapons(bitmask);
	}

	// weapon mode index (GunKeys::WeaponModes), or -1 for none
	void setSelectedWeapon(int weapon) {
		this->hook.setSelectedWeapon(weapon);
	}
};
