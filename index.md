## Diamond Engine

Diamond Engine is an engine from the "Engine Development" subject project for the CITM Bachelor's degree in Video Game Design and Development. Also hoping to be used as the base engine for the "Project 3" subject on the same degree. 

Diamond Engine is a fast C++ 3D game engine, developed with SDL2 and OpenGL. The engine targets optimal memory management, fast starting and loading speeds and simple use. This will provide a fast, simple and powerful tool to prototype good looking games the fastest way possible.

### Collaboration with Unknown Engine Team

Diamond Engine core is under Miquel Suau González's intellectual property. For this assignment and last deploy, we, the team of Unknown Engine have forked from Diamond Engine pre-last release and implemented an Skeleton Animation system.

### Our Team

Unknown Engine team, the one at charge of implementing the Skeletal Animation system is formed by:

- Eudald Garrofé Flix

[GitHub: @Hevne](https://github.com/Hevne)

Eudald has been working on importing animations data into the Engine resources system and saving it under our own custom format, same goes for loading them.
He also worked on the creation of the components, resources and relate the play/stop/pause states to the animations.

<img src="eudald_pic.png"><br>

- Jordi Pardo Gutiérrez

[GitHub: @Jordi-Pardo](https://github.com/Jordi-Pardo)

Jordi mainly focused on linking the animation using the loaded channels and worked on the whole skinning process.
Also, he worked on dividing the animations and the blend between them.

<img src="jordi_pic.png"><br>


### Skeletal Animation System

Our system for this assignment consists on reading the animations inside an FBX, saving them under our own format for further loading into a new Component Animator which will store the keyframes and update our bones.

Later, we use our bones information to link the mesh vertices and normals in order to skin our mesh with the bones movement. Also, we control the state of the current animation playing on the scene using play/stop/pause from the editor.

* Controlling the animation reproduction using the game states
<iframe src="https://giphy.com/embed/NWRjNcmIMKBbKqewdk" width="240" height="183" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/NWRjNcmIMKBbKqewdk">via GIPHY</a><br>

* Changing current animation with keyboard keys
</p><iframe src="https://giphy.com/embed/wZb8WqoFQl90ebgBxf" width="240" height="165" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/wZb8WqoFQl90ebgBxf">via GIPHY</a></p>

### Short Video

<iframe width="560" height="315" src="https://www.youtube.com/embed/UiCn6oklN3A" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### License

Diamond Engine, lightweight 3D game engine software.

Copyright (C) 2020 Miquel Suau Gonzalez

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see https://github.com/MayKoder/Diamond-Engine/blob/master/LICENSE.

Contact at GitHub: https://github.com/MayKoder
Contact at LinkedIn: https://www.linkedin.com/mwlite/in/miquel-suau-gonzalez
Contact at Gmail: sg.miquel@gmail.com
