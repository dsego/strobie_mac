public struct RGB {
  public RGB (float r, float g, float b) {
    this.r = r;
    this.g = g;
    this.b = b;
  }
  public float r;
  public float g;
  public float b;
}

// public struct ColorStop {
//   public RGB (float r, float g, float b, float x) {
//     this.r = r;
//     this.g = g;
//     this.b = b;
//   }
//   public float r;
//   public float g;
//   public float b;
//   public float x;
// }


// public RGB interpolate(ColorStop a, ColorStop b, float pos) {
//   var l = (pos - a.x) / (b.x - a.x);
//   return RGB(
//     a.r + l * (b.r - a.r),
//     a.g + l * (b.g - a.g),
//     a.b + l * (b.b - a.b)
//   );
// }