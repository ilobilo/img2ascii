// Copyright (C) 2025  ilobilo

#include <print>
#include <vector>
#include <cstddef>
#include <cstdio>
#include <filesystem>

#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "stb_image.h"

namespace fs = std::filesystem;

constexpr char map(std::uint8_t brightness, std::uint8_t min, std::uint8_t max)
{
    constexpr char ascii[] = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
    return ascii[(brightness - min) * (sizeof(ascii) - 1) / (max - min)];
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::println(stderr, "img2ascii <filename>");
        return 1;
    }

    std::string file = argv[1];
    if (!fs::exists(file) || !fs::is_regular_file(file))
    {
        std::println(stderr, "regular file '{}' doesn't exist", file);
        return 1;
    }

    winsize win;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win))
    {
        std::println(stderr, "could not get terminal size");
        return 1;
    }

    int w, h, n;
    const auto img = reinterpret_cast<std::uint8_t *>(stbi_load(file.c_str(), &w, &h, &n, 4));
    if (img == nullptr)
    {
        std::println(stderr, "could not load image");
        return 1;
    }

    const auto chunkw = (w + (win.ws_col - 1)) / win.ws_col;
    const auto chunkh = (h + (win.ws_row - 1)) / win.ws_row;

    struct pixel { std::uint8_t r, g, b, lum; };
    auto average = [&](std::size_t startx, std::size_t starty) -> pixel
    {
        std::size_t alphas = 0;
        std::size_t reds = 0;
        std::size_t greens = 0;
        std::size_t blues = 0;
        std::size_t npixels = 0;

        for (std::size_t y = starty; y < std::min(starty + chunkh, static_cast<std::size_t>(h)); y++)
        {
            for (std::size_t x = startx; x < std::min(startx + chunkw, static_cast<std::size_t>(w)); x++)
            {
                std::size_t idx = (y * w + x) * n;
                reds += img[idx];
                greens += img[idx + 1];
                blues += img[idx + 2];
                if (n == 4)
                    alphas += img[idx + 3];
                npixels++;
            }
        }

        reds /= npixels;
        greens /= npixels;
        blues /= npixels;
        if (n == 4)
            alphas /= npixels;

        const auto val = (0.2126 * reds + 0.7152 * greens + 0.0722 * blues);
        const std::uint8_t lum = n == 4 ? alphas ? val : 0 : val;
        return {
            static_cast<std::uint8_t>(reds),
            static_cast<std::uint8_t>(greens),
            static_cast<std::uint8_t>(blues),
            static_cast<std::uint8_t>(lum)
        };
    };

    std::vector<pixel> pixels;
    std::uint8_t max = 0;
    std::uint8_t min = 255;

    for (std::size_t y = 0; y < static_cast<std::size_t>(h); y += chunkh)
    {
        for (std::size_t x = 0; x < static_cast<std::size_t>(w); x += chunkw)
        {
            const auto val = average(x, y);
            max = std::max(max, val.lum);
            min = std::min(min, val.lum);
            pixels.push_back(val);
        }
    }

    std::size_t i = 0;
    for (std::size_t y = 0; y < static_cast<std::size_t>(h); y += chunkh)
    {
        for (std::size_t x = 0; x < static_cast<std::size_t>(w); x += chunkw)
        {
            auto [r, g, b, lum] = pixels[i++];
            std::print("\033[38;2;{};{};{}m{}", r, g, b, map(lum, min, max));
        }
        std::println();
    }

    stbi_image_free(img);

    return 0;
}
