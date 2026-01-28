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

If the outfit loading feature is enabled, then the game loads files from asset folders (ART, MODELS, ANIMATIONS etc) *before* loading from the game's POD archives. This is because the game defines two asset loading functions, with the second as a fallback for the first. (Some modders have already figured out that deleting files from the POD archives will allow them to be loaded from folders.) The outfit loading feature swaps the priority of the asset loading functions. This means that asset replacement mods that previously required packing the files into the game's POD archives should work by placing the files into the appropriate folders in the game directory. This opens the door for easier distribution and installation of mods such as sound replacers, animation replacers, and HD texture packs.

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