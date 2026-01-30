# Gunsucked Mod - for Bloodrayne 2 Terminal Cut

Gunsucked Mod is a rebalance of Rayne's guns that increases the damage and fire rate while lowering ammunition cost of weapon modes across the board, with the goal of making the game play more like a shooter.

Aside from the weapon rebalance, other features include:

- A photo mode
- Hotkeys to equip weapon modes
- Mousewheel down equips previous weapon
- Super slow mode
- No Hud toggle
- Load outfit mods from a mods folder (no POD packing necessary)

## Installation

Place GunsuckedLauncher.exe, Gunsucked.dll, and gunsucked.ini into your Bloodrayne 2: Terminal Cut directory. Launch the game via GunsuckedLauncher.exe.

## Configuration

Each feature is toggleable and configurable and can be modified in gunsucked.ini. Features can be enabled and disabled completely, and values such as camera sensitivity and the rebalanced properties of each weapon mode may be changed, along with key bindings. To define key bindings, the mod uses Win32 virtual key codes. Go to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes for a list of valid codes (however, only keyboard keys have been tested. Gamepad buttons were not tested and shouldn't be expected to work.)

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


## Weapon Changes

These are the modded properties of each weapon mode compared to the vanilla values. Some of the unmodded values appear incorrect (Blood Bomb and Blood Flame have their damage *reduced* at levels 2 and 3) but these values were extracted from hardcoded values in the game's executable.

### Blood Shot

- Damage Level 1: 75 (default 50)
- Damage Level 2: 90 (default 55)
- Damage Level 3: 112.5 (default 60)

- Fire Rate Level 1: 5 (default 4)
- Fire Rate Level 2: 5.5 (default 5)
- Fire Rate Level 3: 6.5 (default 6)

- Range: 120 (default 80)

- Ammo Cost Level 1: 5.56 (default 25)
- Ammo Cost Level 2: 5 (default 18)
- Ammo Cost Level 3: 4.55 (default 15)

### Blood Stream

- Damage Level 1: 75 (default 50)
- Damage Level 2: 80 (default 55)
- Damage Level 3: 85 (default 60)

- Fire Rate Level 1: 12 (default 8)
- Fire Rate Level 2: 16 (default 10)
- Fire Rate Level 3: 20 (default 12)

- Range: 100 (default 80)

- Ammo Cost Level 1: 11.11 (default 25)
- Ammo Cost Level 2: 8.33 (default 22)
- Ammo Cost Level 3: 6.67 (default 20)

### Blood Spray

- Damage Level 1: 20 (default 13.33)
- Damage Level 2: 33 (default 8.33)
- Damage Level 3: 45 (default 10)

- Fire Rate Level 1: 1.5 (default 1)
- Fire Rate Level 2: 1.66 (default 1)
- Fire Rate Level 3: 1.75 (default 1)

- Range: 80 (default 80)

- Ammo Cost Level 1: 33.33 (default 100)
- Ammo Cost Level 2: 28.57 (default 50)
- Ammo Cost Level 3: 25 (default 50)

### Blood Bomb

- Damage Level 1: 200 (default 100)
- Damage Level 2: 275 (default 50)
- Damage Level 3: 350 (default 50)

- Fire Rate Level 1: 1 (default 1)
- Fire Rate Level 2: 1.25 (default 2)
- Fire Rate Level 3: 1.5 (default 1)

- Range: 80 (default 50)

- Ammo Cost Level 1: 50 (default 200)
- Ammo Cost Level 2: 40 (default 80)
- Ammo Cost Level 3: 33.33 (default 67)

### Blood Flame

- Damage Level 1: 100 (default 100)
- Damage Level 2: 120 (default 50)
- Damage Level 3: 140 (default 50)

- Fire Rate Level 1: 1 (default 1)
- Fire Rate Level 2: 1.2 (default 1)
- Fire Rate Level 3: 1.4 (default 1)

- Range: 25 (default 10)

- Ammo Cost Level 1: 66.67 (default 200)
- Ammo Cost Level 2: 55.56 (default 80)
- Ammo Cost Level 3: 45.45 (default 67)

### Blood Hammer

- Damage Level 1: 400 (default 300)
- Damage Level 2: 500 (default 325)
- Damage Level 3: 600 (default 350)

- Fire Rate Level 1: 1 (default 1)
- Fire Rate Level 2: 1.1 (default 1)
- Fire Rate Level 3: 1.2 (default 1)

- Range: 250 (default 200)

- Ammo Cost Level 1: 83.33 (default 300)
- Ammo Cost Level 2: 71.43 (default 131)
- Ammo Cost Level 3: 62.5 (default 116)

## Using Photo Mode

Photo mode is mostly self explanatory. Note it can also be used in conjunction with Super Slow Mode. If Super Slow Mode is enabled, then you can enter photo mode, frame your shot, and then toggle Super Slow Mode to unfreeze the game (to slow motion) while maintaining free camera control. Toggling Super Slow Mode again will return the game to its normal time factor, so you can retain free camera control at regular game speed.

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

If the outfit loading feature is enabled, then the game loads files from asset folders (ART, MODELS, ANIMATIONS etc) *before* loading from the game's POD archives. This is because the game defines two asset loading functions, one for POD archives and one for loose files, with the second as a fallback for the first. (Some modders have already figured out that deleting files from the POD archives will allow them to be loaded from folders.) The outfit loading feature swaps the priority of the asset loading functions. This means that asset replacement mods that previously required packing the files into the game's POD archives should work by placing the files into the appropriate folders in the game directory. This opens the door for easier distribution and installation of mods such as sound replacers, animation replacers, and HD texture packs, as well as direct editing of the game's script files.

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