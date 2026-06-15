{
  description = "Ambiente di sviluppo Rust per Retro-Game";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Toolchain Rust
            cargo
            rustc
            rustfmt
            clippy

            # Dipendenze di compilazione e di sistema per Game Dev
            pkg-config
            udev
            alsa-lib
            vulkan-loader
            
            # Librerie X11/Wayland comuni per la gestione delle finestre (Winit)
            xorg.libX11
            xorg.libXcursor
            xorg.libXi
            xorg.libXrandr
            libxkbcommon
          ];

          # Trucco fondamentale per far trovare le librerie dinamiche a Rust su Arch tramite Nix
          shellHook = ''
            export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${with pkgs; lib.makeLibraryPath [
              vulkan-loader
              xorg.libX11
              xorg.libXcursor
              xorg.libXi
              xorg.libXrandr
              libxkbcommon
              alsa-lib
              udev
            ]}"
            echo "🎮 Ambiente Retro-Game in Rust pronto all'azione!"
          '';
        };
      });
}
