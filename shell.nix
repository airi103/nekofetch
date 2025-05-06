{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  buildInputs = [
    pkgs.gcc
    pkgs.gdb
    pkgs.glibc
    pkgs.binutils
    pkgs.gnumake
    pkgs.pkg-config

    # Add libraries your project uses here:
    pkgs.libsixel
  ];

  # Optional: set some helpful environment variables
  shellHook = ''
    export CC=gcc
  '';
}
