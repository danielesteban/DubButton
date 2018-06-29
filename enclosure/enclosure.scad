WALL = 4;
WIDTH = 40 + (WALL * 2);
LENGTH = 80 + (WALL * 2);

module Box(width, length, height, corner = 0.1) {
  scale([width, length, height]) {
    minkowski() {
      cube([1 - corner, 1 - corner, 0.5], center = true);
      cylinder($fn=64, d=corner, h=0.5, center = true);
    }
  }
}

color("Blue") {
  rotate([90, 0, 0]) {
    difference() {
      translate([0, 0, WALL * 2.5]) {
        Box(WIDTH, LENGTH, WALL * 5);
      }
      translate([0, 0, WALL * 1.5]) {
        Box(WIDTH - WALL * 4, LENGTH - WALL * 4, WALL * 5);
      }
      translate([0, (LENGTH * 0.5) - WALL, WALL]) {
        rotate([90, 0, 0]) {
          Box(WIDTH - (WALL * 4.0), WALL * 6, WALL * 4, 0.6);
        }
      }
      translate([0, WALL, WALL * 1.5]) {
        rotate([90, 0, 0]) {
          Box(WIDTH - WALL * 2, WALL, LENGTH, 0.6);
        }
      }
    }
  }
}
