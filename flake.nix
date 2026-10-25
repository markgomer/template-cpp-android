{
  description = "C++ Android (NDK) dev environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.allowUnfree = true;
          config.android_sdk.accept_license = true;
        };

        buildToolsVersion = "36.0.0";

        toolChain = with pkgs; [
          # C/C++ Toolchain
          gcc
          gnumake
          cmake
          ninja
          pkg-config
        ];

        # Desktop libraries for local compilation and LSP header resolution
        desktopLibs = with pkgs; [
          libGL
          libX11
          wayland
          libxkbcommon
        ];

      in {
        # DEVSHELL (nix develop)
        # =======================================================
        devShells.default = pkgs.mkShell rec {
          buildInputs = toolChain ++ desktopLibs;

          ANDROID_HOME = "/home/majunior/Android";
          ANDROID_NDK = "/home/majunior/Android/ndk/28.2.13676358";
          JAVA_HOME = "/home/majunior/.local/share/asdf-vm/installs/java/openjdk-21";
          LIBRARY_PATH = pkgs.lib.makeLibraryPath desktopLibs;
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath desktopLibs;
          GRADLE_OPTS = "-Dorg.gradle.project.android.aapt2FromMavenOverride=${ANDROID_HOME}/build-tools/${buildToolsVersion}/aapt2";

          shellHook = ''
            export PATH="$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/build-tools/36.0.0:$PATH"
            echo "C++ Android shell active."
            exec zsh
          '';
        };
      });
}
