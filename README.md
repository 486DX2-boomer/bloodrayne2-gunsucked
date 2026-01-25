# Gunsucked Mod - for Bloodrayne 2 Terminal Cut

Gunsucked Mod started off as a rebalance of Rayne's ranged combat. A lot of thought and effort was put into Rayne's Carpathian Dragons, but in the end, they were made so weak, with so little ammunition, that outside of their required use in boss fights, I never used her guns. Gunsucked Mod increases the damage, fire rate, and available ammunition of each weapon mode to make Rayne's guns formidable and usable weapons throughout the game, with the goal of making the game play more like a shooter.

Along the way, a few other features snuck in:

- A photo mode
- Hotkeys to equip weapon modes
- Mousewheel down equips previous weapon
- Super slow mode
- No Hud toggle
- Load outfit mods from a mods folder (no POD packing necessary)

## Installation

Place GunsuckedLauncher.exe, Gunsucked.dll, and gunsucked.ini into your Bloodrayne 2: Terminal Cut directory. Launch the game via GunsuckedLauncher.exe.

## Configuration

Each feature is toggleable and configurable, so if you want a vanilla experience with quality-of-life features, or want to use outfit mods, or want to use wildly overpowered cheat guns, or any combination thereof, open gunsucked.ini and change the values to your tastes. To define keybindings, the mod uses Win32 virtual key codes. Go to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes for a list of valid codes (however, only keyboard keys have been tested. Gamepad buttons were not tested and shouldn't be expected to work.)

### Default Keybinds

| Function | Key |
|----------|-----|
| **Toggles** | |
| Toggle Photo Mode | F7 |
| Toggle Super Slow Mode | F8 |
| Toggle HUD | F9 |
| **Photo Mode Camera** | |
| Move Left / Right | [ and ] |
| Move Down / Up | , and . |
| Move Backward / Forward | ; and ' |
| Pitch Up / Down | Up Arrow / Down Arrow |
| Yaw Left / Right | Left Arrow / Right Arrow |
| Roll Left / Right | Insert / Delete |
| Decrease / Increase FOV | Page Down / Page Up |
| **Weapon Hotkeys** | |
| Blood Shot | 5 |
| Blood Stream | 6 |
| Blood Spray | 7 |
| Blood Bomb | 8 |
| Blood Flame | 9 |
| Blood Hammer | 0 |
| Previous Weapon | - |
| Previous Weapon | Mouse Wheel Down |

## Loading Outfit Mods

If you have downloaded outfit mods from KillerExe_01's excellent mod preservation project on Gamebanana, available at https://gamebanana.com/games/20657, then you will see them packaged in this format:

```
[BR2L]Outfit Standard - Aeon Flux v1.0
	- \ART
	- \MODELS
```

Create a folder called `mods` with a subfolder called `outfits` in your Bloodrayne 2 directory:

`Bloodrayne 2 Terminal Cut\mods\outfits`

And then place the outfit mod folders into this directory. Gunsucked Mod will load these from the Extras->Outfits menu from in-game (so you'll need to have unlocked it with a completed save file or with the "unlock outfits" cheat.)

### A note on loading asset replacement mods

If the outfit loading feature is enabled, then the game loads files from asset folders (ART, MODELS, ANIMATIONS etc) *before* loading from the game's POD archives. This means that asset replacement mods that previously required packing the files into the game's POD archives should work by placing the files into the appropriate folders in the game directory.

I have tested this by downloading the Telekinetic Feeding animation mod (https://gamebanana.com/mods/download/537467#FileInfo_1302134) and placing the contained ANIMATIONS folder into the game directory, which worked as expected. I then tried the same with the Bloodrayne 2 HD texture project, but it uses .DDS files instead of .TEX files, so it wasn't compatible. The HD textures are intended to be used with the FSAA Patch, and since the textures are in DDS format the FSAA Patch probably loads the textures via some kind of DirectX hooking. I used DirectXTex to convert the DDS files to TGA, then used PODTools to convert the TGA files to TEX, and this worked; the mod loaded the textures without any POD packing.

## Notes for Modders

The notes/ directory of the repo contains reverse engineering notes documenting game internals, memory structures, and function signatures that may be useful for those wanting to extend this mod or create other mods. Additionally, the codebase includes some headers that aren't fully utilised in the mod itself but could be useful for other projects, such as PlaySound and DisplayMessage (which can display message boxes in the same way that the in-game tutorials do).

## SPECIAL THANKS

A very special thanks to the original Bloodrayne 2 modder *Turfster*, whose reverse engineering work on the POD format and his PODTools utility made this mod possible.

A very special thanks to *KillerExe_01* on GameBanana, who undertook a serious effort to preserve original mods from the Bloodrayne 2 community that otherwise would have been lost. Adding the outfit loading feature would have been impossible without outfit mods to test it.

```
                                                                                                    
                                                                                              .     
                                                                                              :+    
                                                                                              :*.   
                                                                                              -#=   
                                                                                              *%*   
                                                                                             :%%#   
                                                                                             +%%*   
                                                                                            :@@@+   
                                                                                           .%@@@:   
                                                                                          .%@@@#    
             -=.                                                                         .#@@@@.    
              +*:                  ......                                                #@@@@-     
              :##.              .##+=-==+#%-                                            #@@@@=      
               #%=            :+#%%*+=**###%*:                                        .%@@@@+       
               #@+           -*####+--=+**#*#++.                                     .%@@@@=        
               *@#            +***@@%@@@#*#**+.                                     .%@@@@-         
               +@#.           +**%@@@@@@@###*+:                                    -@@@@@=          
               =@#.          -*#*@@@@@@@%######:                                 .*@@@@@=           
               =@%.         .+*#%@@@@@@@@@#####=                                -@@@@@@=            
               =@%.        .=**#%@@@@@@@@@*###%#                              :@@@@@@@*             
               =@%.         .+#*#@@@@@@@@@*##%#+-                            -@@@@@@@%.             
               +@%.          ++***@@@@@@@%***=. =                            -@@@@@@@=              
               +@%:         .=.#*++#@@@@@@#*=.  +                            =@@@@@@:               
               +@@:         :+.=###*%@@@@**=    -.                           *@@@@#.                
               *@@-     =%@@@@@@@@@@@@@@@#+     :.                          =@@@@*                  
               %@@*   .%@@@@@@@@@@%%@@@@@%@@#=. ..          .--            -@@@@#                   
              .@@@+   -+*#%@@@@@%###@@@@@@@@@@@@@#:         +%@%#-.       =@@@@#                    
              :@@@=   -+*#%@@%*+#####@@@@@%@@@@@@@@-        +#*##**.     -@@@@*.                    
              =@@@=   =%@@@@@@@*+*####%%@@@%%@@@@@#@:       :***###=    :@@@@#                      
              .@@@=   -%@@@@@@@*++**#@@@@@@@%%@@@@@@+        .==*@@@#- -@@@@%                       
               -@@=   -#@@@@@@@+=++*#%@@@@%@@%@@@@%##         :#%*@@@@@%##@#                        
                @@=   -*@@@@#%*+==+*####**++@@@@@%**+:      .=**+-+@@@@@#%#                         
                @@+   -#@@@@*%@%%#*++++===-%@@@@#+===*.   +@%**==  #@@@@@%:                         
                @@+   -#@@@@*@@@@@#%+=----%@@@#=---=@@+=@@@@%@+#-   %@##@=                          
                @@*   =#@@@%+=@@%%%**#%@%#%@@@#+=*#%@@%@@@@@@@@%.   +@@@#                           
                %@*  .=#@@@*: +@%**+**%@@%%@@@+=:-##@@@@@@@@*@#.   :@@@%                            
                %@*  :+%@@%+   %#++**#@@@@@@@#+-  +#%%@@@%#%@@@@*=+@@@@-                            
                %@#  :*#@%#-   -#****#@@@@@@@@@=  .##%%#@@@%*@@#+*@@@-                              
                %@%  :*#@%@=    *****%@@@@%@@@@:   .%##=--:. =#***@@:                               
              +@@@@. :*#@%@=   -*####%@@@@%%@%@:     -=-:    :==*##:                                
             *@@@@@% :+#@#@+  -+*#%@@@@@@%###%@=                                                    
         :=%@@@@@@@+.*@@@@@+ :**#%@@@@@@@%#**%@%                                                    
        +@@%=...@@@*.@@@@@@@-#%#%@@@@@@@%#***#@@:                                                   
         ..     .*@% #%%@#%@@@@#@@@@@@@@%#****@@%                                                   
                 +@%+%#%@##@@@@%@@@@@@%##*****%@@+                                                  
                 =@@@@@@@@@@@@@@@@@@%#******++*@@@:                                                 
                 =@@@@@@@@@@@@@@@@@%##****+++++#@@+                                                 
                 =*%@%**@#%%@@@@@@%%##****+++++*@@%                                                 
                 =@#+.=*@%@@@@@@@@%##***********#@@:                                                
                 .=   =*#%@@@@@@@@%#************#%@+                                                
                      -+***@@@@@@@%#*************%@*                                                
                      =+*%##%@@@@@@#*******###***%@%                                                
                     :++%%@@@@@@@@%#***********+#@@@-                                               
                     :*##@@@@@@@@@#************+#@@@%                                               
                     -%#%#@@@@@@@%#+*****+*****+#@@@@:                                              
                     #@@@%%@@@@@@#+=***+**++**+*#@@@@*                                              
                    -@@@@@@@@@@#**-:*****++++*+*%@#*#%.                                             
                   .%@@@@@@@@@@=-::.+***+++++++*%@@@@#:                                             
                   -@@@@@@@@@@@#*:::=***+++++*+*%@@@#%-                                             
                  .%@@@@@@@@@@@*---:=*+++++++++*%@@@@%-                                             
                  -@@##@@@@@@@@+----=**++++++++*@@@@@@+                                             
                  =@#+#@@@@@@@@====-=++++++++++*@@@@%%-                                             
                  *@**@@@@@@@@@+===-+++++++++++*%##**%-                                             
                  #@*#@@@@%@@@@+====+++++++++++*%%##*#:                                             
                 .#**%@@@#*@@@@+====++++++++++++%%%#*#.                                             
                 :#*#%@@@@@@@@@+====++++++++++++%%%**#.                                             
                .*##%@@@**#@%@@*====++++++++++++%%%#**.                                             
                :@*##@@**+==#@@+===+++++++++++++##%%**.                                             
               .%###*@@%#=++*@@%==-+++++++++++++#%%%*#-                                             
               %@###%@@@@=++*@@*===+++++++++++++#%%#**=                                             
              +%**#%%@@@*=++*%*====+++++++++++++#%%##*-                                             
             :#**##**@@@:=+++%+====+++++++++++++%%#***-                                             
             *#*#%%*=== .=+++#+==-=+++++++++++++#%###**.                                            
            :%*#%##*=*- .=+++#+==-=+++++++++++++#%%%##@-                                            
            ######*+++. .=+++#+=--=+++++++++++++#%%##*%#                                            
           :@*##%#*+*=  :=+++**===++++++++++++++##%%%#*@.                                           
           +*#%%#*+*+.  :=+++**===++++++++++++++@#%%%#*%+                                           
           **#%%#***-   .=++++====+++++++++++++-*%%%%%#*%                                           
           #*#%#*+*+    :=++++====+++++++++++++-.*###%#*%.                                          
          :**#%#***.    :=+++====++++++++++++++- :*%%%#*#-                                          
          -*#%##**-     -=+++====++++++++++++++-  -#%%%#*+                                          
          =*#%#*+=      -=+++====++++++++++++++-   +#%%#**.                                         
          +*#%#*+:      -=+++====++++++++++++++-    *#%%##-                                         
          +#%%#*=       =++++====*+++++++++++++-    .#%%##=                                         
         .+#%#**        =+++++===++++++++++++++-     +%%%#*                                         
         :*#%#*-        =++++====++++++++++++++-     :*%###-                                        
         =##%*+         =++++===+++++++++++++++-     .+#%###                                        
        .##%#+.         =++++===+++++++++++++++-      :*%###-                                       
        =@##*+.         =++++===+++++++++++++++-      +%####*                                       
        %%#@@*         .=++++===+++++++++++++++-     +%##%@%#-                                      
        @%%@%:         .=++++===+++++++++++++++-    .@#%###%#*:                                     
       -@#%%%:         .=+++====**+++++++++++++-     -#%%%@@@#*.                                    
       *@#@@@:         .=+=+====*++++++++++++++-     .+%##%@@@%+                                    
       ###%%%:         :========*****++***+++++=      -%#*#%@@@%:                                   
       #*#@%@-         -========+***+*******++**      .#%- +@@@%%-                                  
       @##@@@-         -========+***********+**#.      *%-  =%@@@%*:                                
      .@##%@@=        .-========+******###*****%-      -%-   +@%%@@%*:                              
      .#*#%%@=       .--=--=====:+*****####***#%*      .#=   =#%%%%#+:                              
      .+*##*=        :=+++++**##*+#############%@:      *=   +#%#%%#+:                              
      .++*#+-     =%**#####%####*###########%%%%@#.     --   .-====-:                               
       .=*#+:       .-+==----:.  :***#%%%%@@@@@@@@+                                                 
                                 .+####**++=++*%@@%-                                                
                               -=++++*#%@@@@@@@@@@@%#=:                                             
                               :=-:....:+#%@@@@@%+-:......                                          
```