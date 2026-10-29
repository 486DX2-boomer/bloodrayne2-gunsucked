#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>
#include "Config.h"

// Reads individual files out of the game's POD archives
// we need this for the weapon wheel, to grab the unused weapon wheel art and draw it
// POD3 layout (all little endian, nothing is compressed):
// 0x000  char[4]  "POD3"
// 0x058  uint32   file count
// 0x108  uint32   index offset
// index: file count entries of 20 bytes { name offset, size, data offset, timestamp, checksum }
// the string table (null terminated names like "ART\HUD.TEX") follows directly after the index

class PodArchive {
private:
	static constexpr uint32_t fileCountOffset = 0x58;
	static constexpr uint32_t indexOffsetOffset = 0x108;
	static constexpr uint32_t headerSize = 0x120;
	static constexpr uint32_t entrySize = 20;

	struct Entry {
		uint32_t nameOffset;
		uint32_t size;
		uint32_t dataOffset;
		uint32_t timestamp;
		uint32_t checksum;
	};
	static_assert(sizeof(Entry) == entrySize, "POD3 index entries are 20 bytes");

public:
	// podPath for example "W32ART.POD". fileName is the path inside the archive such as "ART\\HUD.TEX"
	static bool readFile(const char* podPath, const char* fileName, std::vector<uint8_t>& out) {
		std::ifstream pod(podPath, std::ios::binary);
		if (!pod) {
			DEBUG_LOG("PodArchive: couldn't open " << podPath);
			return false;
		}

		uint8_t header[headerSize];
		if (!pod.read(reinterpret_cast<char*>(header), headerSize) || std::memcmp(header, "POD3", 4) != 0) {
			DEBUG_LOG("PodArchive: " << podPath << " is not a POD3 archive");
			return false;
		}

		uint32_t fileCount;
		uint32_t indexOffset;
		std::memcpy(&fileCount, header + fileCountOffset, sizeof(fileCount));
		std::memcpy(&indexOffset, header + indexOffsetOffset, sizeof(indexOffset));

		std::vector<Entry> entries(fileCount);
		pod.seekg(indexOffset);
		if (!pod.read(reinterpret_cast<char*>(entries.data()), static_cast<std::streamsize>(fileCount) * entrySize)) {
			DEBUG_LOG("PodArchive: couldn't read index of " << podPath);
			return false;
		}

		// the string table runs to the end of the file
		std::vector<char> strings((std::istreambuf_iterator<char>(pod)), std::istreambuf_iterator<char>());

		for (const Entry& entry : entries) {
			if (entry.nameOffset >= strings.size()) continue;
			// names are stored uppercase, but compare case-insensitively anyway
			if (_stricmp(&strings[entry.nameOffset], fileName) != 0) continue;

			out.resize(entry.size);
			pod.clear();
			pod.seekg(entry.dataOffset);
			if (!pod.read(reinterpret_cast<char*>(out.data()), entry.size)) {
				DEBUG_LOG("PodArchive: couldn't read " << fileName << " from " << podPath);
				return false;
			}
			return true;
		}

		DEBUG_LOG("PodArchive: " << fileName << " not found in " << podPath);
		return false;
	}
};
