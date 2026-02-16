ALWAYS use incomplete types in headers.
PREFER minimal headers.
NO smart pointers.
c++11 only.
NO member variable default values allowed in headers. put it in the constructor.
NO SFML vector types in headers.
NO code in headers. (no inline template methods)
NO files above 1000 lines.
engine STAYS at 640x480 resolution forever.
ALWAYS compartmentalize/abstract use of libraries.