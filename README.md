# What is peaEngine?
peaEngine is a simple software renderer for basic .obj 3d models (only vertices and faces) meant for learning or quick viewing

 ## Features
* **Support for .obj files:** load any standard model

* **Shading:** lambertian shading is used by default, providing better depth perception

## Usage
`./peaEngine yourModel.obj [-mode] [-a]`

**Modes:**
* **-p:** only view vertices as points
* **-w:** wireframe only
* **-m:** only the model, with distance "fog"
* **-mm:** the model and the mesh
* **-ms:** model with proper shading

**Anti-aliasing**
Gaussian, vaseline-looking AA.
Used with **-a**. It tanks performance.

## Controls
* **WASD** to move (Q and E for up and down)
* **Arrows** to look around
