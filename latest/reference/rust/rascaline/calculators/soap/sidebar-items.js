window.SIDEBAR_ITEMS = {"enum":[["CutoffFunction","Possible values for the smoothing cutoff function"],["RadialBasis","Radial basis that can be used in the spherical expansion"],["RadialScaling","Implemented options for radial scaling of the atomic density around an atom"]],"struct":[["GtoParameters","Parameters controlling GTO radial basis"],["GtoRadialIntegral",""],["HyperGeometricParameters",""],["HyperGeometricSphericalExpansion","Computes the G function and its derivative for all possible values of `l < l_max + 1` and `n < n_max` with `a = (n + l + 3) / 2` and b = `l + 3/2` using recurrence relationships between different n/l values to speedup the full calculation."],["PowerSpectrumParameters","Parameters for SOAP power spectrum calculator."],["RadialSpectrumParameters","Parameters for the SOAP radial spectrum calculator."],["SoapPowerSpectrum","Calculator implementing the Smooth Overlap of Atomic Position (SOAP) power spectrum representation of atomistic systems."],["SoapRadialSpectrum","Calculator implementing the Radial spectrum representation of atomistic systems."],["SphericalExpansion","The actual calculator used to compute SOAP spherical expansion coefficients"],["SphericalExpansionParameters","Parameters for spherical expansion calculator."],["SphericalHarmonics","Compute a full set of spherical harmonics at given positions"],["SphericalHarmonicsArray","Array storing data for `0 <= l <= l_max`, `-l <= m <= l`. This type implements `Index<[isize; 2]>` and `IndexMut<[isize; 2]>` to allow writing code like"],["SplinedRIParameters","Parameters for computing the radial integral using Hermit cubic splines"],["SplinedRadialIntegral","`SplinedRadialIntegral` allows to evaluate another radial integral implementation using cubic Hermit spline."]],"trait":[["RadialIntegral","A `RadialIntegral` computes the radial integral on a given radial basis."]]};