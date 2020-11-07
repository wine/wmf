# wmf
wine's mini fetch

*this project was made for [void linux](https://voidlinux.org) but can (hopefully) easily be changed to run on other distributions.*

![](_res/showcase.png)

## configuration
simple configuration can be done in `src/config.h`, for anything more "complex"
look at `src/main.c`.

after editing the config recompile and reinstall.

## installation
`git clone https://github.com/wine/wmf && cd wmf && make && sudo make install`

## roadmap

- [x] ascii logo
- [x] packages
- [ ] cpu
- [x] gpu
- [x] ram
