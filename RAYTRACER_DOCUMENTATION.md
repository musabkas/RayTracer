# Ray Tracer: Complete Technical Overview

## 1. Architecture & Pipeline

Your ray tracer follows a standard **Monte Carlo path tracing pipeline**:

```
Scene Setup → Render Loop → Primary Ray Generation → Ray Tracing → Shading → Image Output
```

---

## 2. Rendering Equation & Path Tracing

The fundamental equation solved by your tracer is the **rendering equation**:

$$L_o(\mathbf{x}, \omega_o) = L_e(\mathbf{x}, \omega_o) + \int_{\Omega} f_r(\mathbf{x}, \omega_i, \omega_o) L_i(\mathbf{x}, \omega_i) (\mathbf{n} \cdot \omega_i) \, d\omega_i$$

Where:
- $L_o$ = outgoing radiance (light leaving a surface)
- $L_e$ = emitted light (from light sources)
- $f_r$ = BRDF (bidirectional reflectance distribution function)
- $L_i$ = incoming radiance
- $\mathbf{n}$ = surface normal
- $\omega_i$, $\omega_o$ = incoming and outgoing directions
- $\Omega$ = hemisphere of directions

### Path Tracing Approximation

Your tracer estimates this integral using **Monte Carlo integration with importance sampling**:

$$L_o \approx L_{\text{direct}} + \frac{1}{N} \sum_{k=1}^{N} \frac{f_r(\omega_k) L_o(\text{bounce}) (\mathbf{n} \cdot \omega_k)}{p(\omega_k)}$$

---

## 3. Main Render Loop

**File:** `raytracer.cpp`

```cpp
For each pixel (x, y):
    pixel_color = 0
    For each sample ray:
        ray = sampler.get_rays(x, y)
        shade = tracer.trace_ray(ray, world)
        pixel_color += weight × shade
    image.set_pixel(x, y, pixel_color)
```

The pixel color is computed as a **weighted average** of multiple sample rays:

$$C(x,y) = \frac{1}{N} \sum_{i=1}^{N} L_i$$

Where $N$ is the number of samples per pixel (16 in your default setup).

---

## 4. Ray Generation & Sampling

**Files:** `samplers/Jittered.cpp`, `cameras/Camera.hpp`

### Jittered Sampling

Your sampler uses **stratified jittering** to reduce aliasing:

1. Divide pixel into $\sqrt{N} \times \sqrt{N}$ sub-grid cells
2. For each cell, place one sample at:
$$\mathbf{p}_{ij} = \left( \frac{i + u}{n}, \frac{j + v}{n} \right)$$

Where:
- $i, j$ = grid indices
- $u, v \sim \text{Uniform}[0,1)$ = random jitter offsets
- $n = \sqrt{N}$ = samples per side

### World-Space Conversion

Pixel coordinates are transformed to world space:

$$\mathbf{p}_{\text{world}} = \mathbf{p}_{\text{top-left}} + \text{offset}_x \cdot \text{pixel\_width} + \text{offset}_y \cdot \text{pixel\_height}$$

Primary rays are then generated via the camera:

$$\mathbf{r}(t) = \mathbf{o} + t \mathbf{d}$$

Where $\mathbf{o}$ is the ray origin and $\mathbf{d}$ is the normalized direction.

---

## 5. Ray-Scene Intersection

**Files:** `geometry/Sphere.cpp`, `geometry/BVHTree.cpp`

### Ray-Sphere Intersection

For a sphere with center $\mathbf{c}$ and radius $r$:

$$(\mathbf{o} + t\mathbf{d} - \mathbf{c}) \cdot (\mathbf{o} + t\mathbf{d} - \mathbf{c}) = r^2$$

Expanding into quadratic form:

$$At^2 + Bt + C = 0$$

Where:
- $A = \mathbf{d} \cdot \mathbf{d}$
- $B = 2(\mathbf{d} \cdot (\mathbf{o} - \mathbf{c}))$
- $C = (\mathbf{o} - \mathbf{c}) \cdot (\mathbf{o} - \mathbf{c}) - r^2$

Solutions:
$$t = \frac{-B \pm \sqrt{B^2 - 4AC}}{2A}$$

Take the **smallest positive** root for the nearest intersection.

### Surface Normal

At intersection point $\mathbf{p} = \mathbf{o} + t\mathbf{d}$:

$$\mathbf{n} = \frac{\mathbf{p} - \mathbf{c}}{|\mathbf{p} - \mathbf{c}|}$$

### Acceleration: Bounding Volume Hierarchy (BVH)

**File:** `geometry/BVHTree.cpp`

Instead of testing all primitives (linear $O(N)$), use a binary tree:

- **Tree Structure:** Each node contains a bounding box (AABB)
- **Construction:** Recursively split primitives along the axis with largest variance
- **Ray Traversal:** Depth-first search, skip subtrees if ray misses AABB
- **Complexity:** $O(\log N)$ average case for ray-scene intersection

---

## 6. Material & BRDF: Diffuse (Lambertian)

**Files:** `materials/Diffuse.cpp`, `BRDF/Lambertian.cpp`

### Lambertian BRDF

The Lambertian reflectance model is frequency-independent:

$$f_r(\omega_i, \omega_o) = \frac{\rho}{\pi}$$

Where $\rho$ is the **albedo** (reflectance coefficient).

This represents **diffuse surfaces** with view-independent brightness.

### Two-Component Shading

Your path tracer splits shading into **direct** and **indirect** components:

$$L_o = L_{\text{direct}} + L_{\text{indirect}}$$

#### Direct Lighting (Next Event Estimation)

For each light $L$ in the scene:

$$L_{\text{direct}} = \sum_{L} f_r(\omega_L) L(\omega_L) (\mathbf{n} \cdot \omega_L) \cdot \text{visibility}(\mathbf{p}, L)$$

Where:
- $\omega_L$ = normalized direction toward light
- $\text{visibility}$ = 1 if unoccluded, 0 if in shadow (via shadow ray)

#### Indirect Lighting (Path Continuation)

**1. Hemispherical Sampling**

Generate a uniformly random direction in the hemisphere using spherical coordinates. First, create an orthonormal frame $(u, v, w)$ where $w = \mathbf{n}$:

$$v = \frac{(\mathbf{k} \times \mathbf{n})}{|(\mathbf{k} \times \mathbf{n})|}$$
$$u = v \times w$$

Then generate random direction:
$$r_1, r_2 \sim \text{Uniform}[0,1)$$
$$\phi = 2\pi r_1$$
$$\theta = \arccos(\sqrt{r_2})$$

$$\omega_i = \sin(\theta)\cos(\phi) \, u + \sin(\theta)\sin(\phi) \, v + \cos(\theta) \, w$$

**2. Russian Roulette (Probabilistic Termination)**

Stop bounces with probability $1 - p_s$ to prevent infinite recursion:

$$L_{\text{indirect}} = \begin{cases}
0 & \text{if } \xi > p_s \\
\frac{L_o(\text{bounce})}{p_s} & \text{otherwise}
\end{cases}$$

Where $\xi \sim \text{Uniform}[0,1)$ and $p_s = 0.8$ (default survival probability).

**3. Path Contribution**

The indirect component is:

$$L_{\text{indirect}} = \frac{f_r(\omega_i) L_o(\omega_i \text{ bounce}) (\mathbf{n} \cdot \omega_i)}{p_s}$$

With $f_r(\omega_i) = \frac{\rho}{\pi}$ for Lambertian surfaces.

### Final Shading

$$\boxed{L_o = L_{\text{direct}} + L_{\text{indirect}}}$$

The division by $\pi$ in $f_r$ ensures **energy conservation** on diffuse surfaces.

---

## 7. Ray-Light Interaction

**File:** `lights/PointLight.cpp`

### Point Light Source

A point light at position $\mathbf{p}_L$ with intensity $I$ and color $\mathbf{c}$:

$$L(\mathbf{x} \to \mathbf{p}_L) = I \cdot \mathbf{c}$$

**Direction to light:**
$$\omega_L = \frac{\mathbf{p}_L - \mathbf{x}}{|\mathbf{p}_L - \mathbf{x}|}$$

**Distance:**
$$d = |\mathbf{p}_L - \mathbf{x}|$$

Shadow rays are cast from the surface toward the light to determine occlusion.

---

## 8. Image Formation

**File:** `image/Image.cpp`

Each pixel stores accumulated radiance values. After rendering, pixel values are **tone-mapped and clamped** to $[0, 1]$ before writing as PNG:

$$C_{\text{final}}(x,y) = \text{clamp}\left(\frac{1}{N}\sum_{i=1}^N L_i, 0, 1\right)$$

---

## 9. Complete Data Flow

```
┌─────────────────────────────────────────────────────────┐
│  Pixel (x, y) → Sampler → N Primary Rays              │
├─────────────────────────────────────────────────────────┤
│  Ray → BVH Acceleration → Nearest Hit Point           │
├─────────────────────────────────────────────────────────┤
│  Material.path_shade():                                │
│    ├─ Direct Lighting (Next Event Estimation)        │
│    │  └─ For each light: f_r × L × (n·ω) × visibility
│    └─ Indirect Lighting (Russian Roulette Bounce)    │
│       └─ New ray: f_r × L_o(bounce) / p_s           │
├─────────────────────────────────────────────────────────┤
│  Accumulate: pixel_color += (1/N) × shade            │
├─────────────────────────────────────────────────────────┤
│  Write to Image Buffer                                 │
└─────────────────────────────────────────────────────────┘
```

---

## 10. Key Algorithm: Path Tracing with Importance Sampling

**Summary of the entire process:**

Given a primary ray $\mathbf{r}(t) = \mathbf{o} + t\mathbf{d}$:

1. **Intersect** with scene geometry (BVH acceleration)
2. **If hit** at point $\mathbf{x}$ with normal $\mathbf{n}$ and material $f_r$:
   - Compute $L_{\text{direct}}$: sample all lights, check occlusion
   - With probability $p_s$: generate bounce ray $\mathbf{r}_{\text{bounce}}$
   - Recursively trace $L_{\text{indirect}} = \text{trace}(\mathbf{r}_{\text{bounce}}) / p_s$
   - Return $L_{\text{direct}} + L_{\text{indirect}}$
3. **If no hit**: return background color
4. **Repeat for $N$ samples** and average

The estimator converges to the rendering equation solution as $N \to \infty$.

---

## 11. Performance Optimizations

- **BVH Tree:** $O(\log N)$ ray-scene intersection instead of $O(N)$
- **Jittered Sampling:** Stratified distribution reduces aliasing artifacts
- **Russian Roulette:** Finite expected bounces without hard recursion limit
- **Shadow Rays:** Direct sampling of lights improves convergence
- **Thread-Local RNG:** Safe random number generation for parallel processing

---

## 12. Summary: Key Equations

| Concept | Equation |
|---------|----------|
| **Rendering Equation** | $L_o = L_e + \int f_r L_i (\mathbf{n} \cdot \omega_i) d\omega_i$ |
| **Ray Parameterization** | $\mathbf{r}(t) = \mathbf{o} + t\mathbf{d}$ |
| **Lambertian BRDF** | $f_r = \frac{\rho}{\pi}$ |
| **Path Tracing** | $L_o = L_{\text{direct}} + \frac{L_{\text{indirect}}}{p_s}$ |
| **Ray-Sphere** | $At^2 + Bt + C = 0$ where $A = \mathbf{d} \cdot \mathbf{d}$, etc. |
| **Direct Lighting** | $L_d = \sum_L f_r L(\omega_L) (\mathbf{n} \cdot \omega_L) \cdot \text{vis}$ |
| **Pixel Color** | $C(x,y) = \frac{1}{N}\sum_{i=1}^N L_i$ |

