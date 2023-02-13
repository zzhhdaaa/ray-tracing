# Ray Tracing Renderer

This is a repo for recording my self-study path of creating a ray-tracing renderer from scratch. 

## Getting Started
Once you've cloned, the Visual Studio 2022 solution file is in the root named `RayTracing.sln`. And all source code of the renderer is under `RayTracing/src/`. 

## Reference
For reference, I am following the book series ***Ray Tracing in One Weekend, The Next Week, and The Rest of Your Life*** on GitHub as well as the video series ***Ray Tracing, The Cherno*** on YouTube, and then the big guy ***Physically Based Rendering:From Theory To Implementation*** by Matt Pharr, Wenzel Jakob, and Greg Humphreys. Many thanks to these authors, I have already learned a lot even though I just started.
###### Book Series [Ray Tracing in One Weekend, The Next Week, and The Rest of Your Life](https://raytracing.github.io/)
###### Video Series [Ray Tracing, from The Cherno](https://youtube.com/playlist?list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl)
###### The Big Guy [Physically Based Rendering: From Theory To Implementation (3rd Edition)](https://www.pbr-book.org/)

## GUI Template Usage
Although called "from scratch", I use an app template from `TheCherno/WalnutAppTemplate.git` to help me handle GUI and visualize the render output interactively. This helps me get rid of things that are not so important at this point and focus on the core parts. 

## Current Stage
Implementing very basic refraction. 

- Add Opacity in the Material class, as a random scaler to decide whether the ray should reflect or refract
- Use incident direction, normal, and index of refraction(IOR) to calculate the refracted direction
- Refactor the Solve Quadratic function and its result struct for clarification
- When a ray hits, calculate whether it hits in the front face or the back face

The glass spheres still look odd when compared to a real-world one. 

https://user-images.githubusercontent.com/91817338/218551512-f9477720-35ac-44d9-9295-d0570a9c7aa4.mp4

