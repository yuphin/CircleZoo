# CircleZoo

![0](/res/img.png)

This is a mini code zoo that explores different circle and ellipse drawing algorithms. The goal is to check their behavior and get a rough estimate on their performance.

## Methods

### Circles
- **Midpoint Circle Algorithm**  
  Standard integer-based algorithm that draws a circle using symmetry across 8 octants.

- **DDA (Digital Differential Analyzer)**  
  Sweeps X and adjusts Y based on the derivative.

- **Adaptive Analytical Method**  
  Uses (`acos`, `asin`) to adjust (Δθ) such that each new point lies approximately 1 pixel away.

- **Jesko’s Variant of the Midpoint Algorithm**  
  A variant of the midpoint method. Interestingly it was slower on my tests. [Ref](https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#Jesko's_Method)

### Ellipses
- **Midpoint Ellipse Algorithm**  
- **Adaptive Analytical Method**  

## Notes
- All methods are benchmarked using `__rdtsc()` on a Ryzen 7950X.
- The code is educational and intentionally kept simple, it's not optimized at all.
- Select the drawing algorithm by setting the `DRAW_METHOD` macro in `main.c`.

## Compiling

Use the Developer Command Prompt for Visual Studio and run `build.bat`

## Results

### Circle Drawing

| Method             | Cycles (approx) |
|--------------------|-----------------|
| Midpoint           | 180             |
| DDA                | 560             |
| Analytical Δθ      | 2800            |
| Jesko’s Midpoint   | 230             |

### Ellipse Drawing

| Method             | Cycles (approx) |
|--------------------|-----------------|
| Midpoint           | 220             |
| Analytical Δθ      | 5000            |