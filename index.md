## Diamond Engine

Diamond Engine is an engine from the "Engine Development" subject project for the CITM Bachelor's degree in Video Game Design and Development. Also hoping to be used as the base engine for the "Project 3" subject on the same degree. 

Diamond Engine is a fast C++ 3D game engine, developed with SDL2 and OpenGL. The engine targets optimal memory management, fast starting and loading speeds and simple use. This will provide a fast, simple and powerful tool to prototype good looking games the fastest way possible.

### Collaboration with Unknown Engine Team

Diamond Engine core is under Miquel Suau González's intellectual property. For this assignment and last deploy, we, the team of Unknown Engine have forked from Diamond Engine pre-last release and implemented an Skeleton Animation system.

### Our Team

Unknown Engine team, the one at charge of implementing the Skeletal Animation system is formed by:

- Eudald Garrofé Flix

Eudald has been working on importing animations data into the Engine resources system and saving it under our own custom format, same goes for loading them.
He also worked on the creation of the components, resources and relate the play/stop/pause states to the animations.

<img src="eudald_pic.png"><br>

- Jordi Pardo Gutiérrez

Jordi mainly focused on linking the animation using the loaded channels and worked on the whole skinning process.
Also, he worked on dividing the animations and the blend between them.

<img src="jordi_pic.png"><br>


### Skeletal Animation System

Our system for this assignment consists on reading the animations inside an FBX, saving them under our own format for further loading into a new Component Animator which will store the keyframes and update our bones.

Later, we use our bones information to link the mesh vertices and normals in order to skin our mesh with the bones movement. Also, we control the state of the current animation playing on the scene using play/stop/pause from the editor.


<iframe src="https://giphy.com/embed/NWRjNcmIMKBbKqewdk" width="240" height="183" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/NWRjNcmIMKBbKqewdk">via GIPHY</a></p>

<iframe src="https://giphy.com/embed/wZb8WqoFQl90ebgBxf" width="240" height="165" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/wZb8WqoFQl90ebgBxf">via GIPHY</a></p>


### Short Video

<iframe width="560" height="315" src="https://www.youtube.com/embed/UiCn6oklN3A" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/Hevne/Diamond-Engine/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
