# resynthesizer

WIP

## Building resynthesizer

### macOS

To compile resynthesizer you must have Xcode installed and you need to make sure the command line tools are setup by running:

```
$ xcode-select --install
```

After that you can build the static library and the examples simply by:

```
$ make
```

Additionally run:
```
$ make test
```

to test the sample images. 

FYI, the test results should look like this:
![preview](./docs/preview.png)

## Building examples

### hello

Minimal build test. Should write `hello` to the stdout.

```
$ make examples/hello
$ examples/hello
```

### ppm

![example-ppm](./docs/example-ppm.png)

Takes in source/mask bitmap and do the image synthesis operation. The dimensions of the source and mask image should match.

```
$ make examples/ppm
$ examples/ppm <source> <mask> <result> <context> <neighbors> <probes>
```

E.g.
```
$ examples/ppm assets/source000.ppm assets/mask000.ppm result000.ppm 1 16 64
```

NOTE: There is a meaningful correlation between **the quality of opration** and the type of fitting method(**context**) described below. The quality of synthesis operation may vary.

| context | method      | shape      | direction |
|---------|-------------|------------|-----------|
| 0       | Patching    | -          | -         |
| 1       | Shuffle     | Random     | Random    |
| 2       | Brushfire   | Concentric | Inward    |
| 3       | Directional | Horizontal | Inward    |
| 4       | Directional | Vertical   | Inward    |
| 5       | Brushfire   | Concentric | Outward   |
| 6       | Directional | Horizontal | Outward   |
| 7       | Directional | Vertical   | Outward   |
| 8       | Squeeze     | Concentric | Both      |

### painter

![example-painter](./docs/example-painter.gif)

Interactive painting demo. Performs synthesis operation on-the-fly.

This example requires [SDL2](https://wiki.libsdl.org/Installation) installed on the build machine, otherwise build will not succeed.

`Makefile` tries to locate the SDL2 using `pkg-config` command. If facing errors, be sure to check if SDL2 is installed properly.

```
$ make examples/painter
$ examples/painter <source>
```
E.g.
```
$ examples/painter assets/source001.ppm
```