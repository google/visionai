/* Goom Project
 * Copyright (C) <2003> Jean-Christophe Hoelt <jeko@free.fr>
 *
 * goom_core.c:Contains the core of goom's work.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/goom/mathtools.h"

float sin256[256] = {
  0, 0.0245412, 0.0490677, 0.0735646, 0.0980171, 0.122411, 0.14673, 0.170962,
  0.19509, 0.219101, 0.24298, 0.266713, 0.290285, 0.313682, 0.33689,
  0.359895, 0.382683, 0.405241, 0.427555, 0.449611, 0.471397, 0.492898,
  0.514103, 0.534998, 0.55557, 0.575808, 0.595699, 0.615232, 0.634393,
  0.653173, 0.671559, 0.689541, 0.707107, 0.724247, 0.740951, 0.757209,
  0.77301, 0.788346, 0.803208, 0.817585, 0.83147, 0.844854, 0.857729,
  0.870087, 0.881921, 0.893224, 0.903989, 0.91421, 0.92388, 0.932993,
  0.941544, 0.949528, 0.95694, 0.963776, 0.970031, 0.975702, 0.980785,
  0.985278, 0.989177, 0.99248, 0.995185, 0.99729, 0.998795, 0.999699, 1,
  0.999699, 0.998795, 0.99729, 0.995185, 0.99248, 0.989177, 0.985278,
  0.980785, 0.975702, 0.970031, 0.963776, 0.95694, 0.949528, 0.941544,
  0.932993, 0.92388, 0.91421, 0.903989, 0.893224, 0.881921, 0.870087,
  0.857729, 0.844854, 0.83147, 0.817585, 0.803208, 0.788346, 0.77301,
  0.757209, 0.740951, 0.724247, 0.707107, 0.689541, 0.671559, 0.653173,
  0.634393, 0.615232, 0.595699, 0.575808, 0.55557, 0.534998, 0.514103,
  0.492898, 0.471397, 0.449611, 0.427555, 0.405241, 0.382683, 0.359895,
  0.33689, 0.313682, 0.290285, 0.266713, 0.24298, 0.219101, 0.19509,
  0.170962, 0.14673, 0.122411, 0.0980171, 0.0735646, 0.0490677, 0.0245412,
  1.22465e-16, -0.0245412, -0.0490677, -0.0735646, -0.0980171, -0.122411,
  -0.14673, -0.170962, -0.19509, -0.219101, -0.24298, -0.266713, -0.290285,
  -0.313682, -0.33689, -0.359895, -0.382683, -0.405241, -0.427555,
  -0.449611, -0.471397, -0.492898, -0.514103, -0.534998, -0.55557,
  -0.575808, -0.595699, -0.615232, -0.634393, -0.653173, -0.671559,
  -0.689541, -0.707107, -0.724247, -0.740951, -0.757209, -0.77301,
  -0.788346, -0.803208, -0.817585, -0.83147, -0.844854, -0.857729,
  -0.870087, -0.881921, -0.893224, -0.903989, -0.91421, -0.92388, -0.932993,
  -0.941544, -0.949528, -0.95694, -0.963776, -0.970031, -0.975702,
  -0.980785, -0.985278, -0.989177, -0.99248, -0.995185, -0.99729, -0.998795,
  -0.999699, -1, -0.999699, -0.998795, -0.99729, -0.995185, -0.99248,
  -0.989177, -0.985278, -0.980785, -0.975702, -0.970031, -0.963776,
  -0.95694, -0.949528, -0.941544, -0.932993, -0.92388, -0.91421, -0.903989,
  -0.893224, -0.881921, -0.870087, -0.857729, -0.844854, -0.83147,
  -0.817585, -0.803208, -0.788346, -0.77301, -0.757209, -0.740951,
  -0.724247, -0.707107, -0.689541, -0.671559, -0.653173, -0.634393,
  -0.615232, -0.595699, -0.575808, -0.55557, -0.534998, -0.514103,
  -0.492898, -0.471397, -0.449611, -0.427555, -0.405241, -0.382683,
  -0.359895, -0.33689, -0.313682, -0.290285, -0.266713, -0.24298, -0.219101,
  -0.19509, -0.170962, -0.14673, -0.122411, -0.0980171, -0.0735646,
  -0.0490677, -0.0245412
};

float cos256[256] = {
  0, 0.999699, 0.998795, 0.99729, 0.995185, 0.99248, 0.989177, 0.985278,
  0.980785, 0.975702, 0.970031, 0.963776, 0.95694, 0.949528, 0.941544,
  0.932993, 0.92388, 0.91421, 0.903989, 0.893224, 0.881921, 0.870087,
  0.857729, 0.844854, 0.83147, 0.817585, 0.803208, 0.788346, 0.77301,
  0.757209, 0.740951, 0.724247, 0.707107, 0.689541, 0.671559, 0.653173,
  0.634393, 0.615232, 0.595699, 0.575808, 0.55557, 0.534998, 0.514103,
  0.492898, 0.471397, 0.449611, 0.427555, 0.405241, 0.382683, 0.359895,
  0.33689, 0.313682, 0.290285, 0.266713, 0.24298, 0.219101, 0.19509,
  0.170962, 0.14673, 0.122411, 0.0980171, 0.0735646, 0.0490677, 0.0245412,
  6.12323e-17, -0.0245412, -0.0490677, -0.0735646, -0.0980171, -0.122411,
  -0.14673, -0.170962, -0.19509, -0.219101, -0.24298, -0.266713, -0.290285,
  -0.313682, -0.33689, -0.359895, -0.382683, -0.405241, -0.427555,
  -0.449611, -0.471397, -0.492898, -0.514103, -0.534998, -0.55557,
  -0.575808, -0.595699, -0.615232, -0.634393, -0.653173, -0.671559,
  -0.689541, -0.707107, -0.724247, -0.740951, -0.757209, -0.77301,
  -0.788346, -0.803208, -0.817585, -0.83147, -0.844854, -0.857729,
  -0.870087, -0.881921, -0.893224, -0.903989, -0.91421, -0.92388, -0.932993,
  -0.941544, -0.949528, -0.95694, -0.963776, -0.970031, -0.975702,
  -0.980785, -0.985278, -0.989177, -0.99248, -0.995185, -0.99729, -0.998795,
  -0.999699, -1, -0.999699, -0.998795, -0.99729, -0.995185, -0.99248,
  -0.989177, -0.985278, -0.980785, -0.975702, -0.970031, -0.963776,
  -0.95694, -0.949528, -0.941544, -0.932993, -0.92388, -0.91421, -0.903989,
  -0.893224, -0.881921, -0.870087, -0.857729, -0.844854, -0.83147,
  -0.817585, -0.803208, -0.788346, -0.77301, -0.757209, -0.740951,
  -0.724247, -0.707107, -0.689541, -0.671559, -0.653173, -0.634393,
  -0.615232, -0.595699, -0.575808, -0.55557, -0.534998, -0.514103,
  -0.492898, -0.471397, -0.449611, -0.427555, -0.405241, -0.382683,
  -0.359895, -0.33689, -0.313682, -0.290285, -0.266713, -0.24298, -0.219101,
  -0.19509, -0.170962, -0.14673, -0.122411, -0.0980171, -0.0735646,
  -0.0490677, -0.0245412, -1.83697e-16, 0.0245412, 0.0490677, 0.0735646,
  0.0980171, 0.122411, 0.14673, 0.170962, 0.19509, 0.219101, 0.24298,
  0.266713, 0.290285, 0.313682, 0.33689, 0.359895, 0.382683, 0.405241,
  0.427555, 0.449611, 0.471397, 0.492898, 0.514103, 0.534998, 0.55557,
  0.575808, 0.595699, 0.615232, 0.634393, 0.653173, 0.671559, 0.689541,
  0.707107, 0.724247, 0.740951, 0.757209, 0.77301, 0.788346, 0.803208,
  0.817585, 0.83147, 0.844854, 0.857729, 0.870087, 0.881921, 0.893224,
  0.903989, 0.91421, 0.92388, 0.932993, 0.941544, 0.949528, 0.95694,
  0.963776, 0.970031, 0.975702, 0.980785, 0.985278, 0.989177, 0.99248,
  0.995185, 0.99729, 0.998795, 0.999699
};
