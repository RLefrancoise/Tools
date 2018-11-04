# Tools
All the tools I developed for games or other stuff.

List of tools:

- [AnimationCreator](#AnimationCreator)

## AnimationCreator

This tool is used to convert sprites to a spritesheet and to create an animation from it.

The initial sprite / animation format can be obtained by using my other tool [Sprite Animation Editor](https://github.com/RLefrancoise/SpriteAnimationEditor "SpriteAnimationEditor"). This format is composed of a list of sprites named "1.png", "2.png", etc..., a file named "anim.txt" and a file named "collisions.txt". All of these must be in the same folder. This folder is the folder that must be given to AnimationCreator to convert to a spritesheet and a XML file. You can see the example folder in the "animations" folder of this tool.

### Dependencies

- SFML 2.1
- Boost (string)
- TinyXML

### Usage

**-all option**
```
AnimationCreator -all <animation_folder> <animation_folder> <animation_folder> ...
```

This option will generate animation for each given folder. The spritesheet will have the same name as the folder.

**single animation usage**

```
AnimationCreator <options> <animation_folder> <spritesheet_name>
```

- *options*: the options to give to the tool. There are no options besides *-all* for now
- *animation_folder*: The path of the animation folder to convert
- *spritesheet_name*: The name of the generated spritesheet