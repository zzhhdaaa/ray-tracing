#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

#include "other/external/stb_image.h"

class Texture
{
public:
	virtual glm::vec3 GetColor(glm::vec2 uv, const glm::vec3& point) const = 0;
};

class SolidColor : public Texture
{
public:
	SolidColor() : m_Color(glm::vec3(1.0f)) {}
	SolidColor(glm::vec3 color) : m_Color(color) {}
	SolidColor(float r, float g, float b) : m_Color(glm::vec3(r, g, b)) {}

	virtual glm::vec3 GetColor(glm::vec2 uv, const glm::vec3& point) const override
	{
		return m_Color;
	}

	void SetColor(const float& r, const float& g, const float& b)
	{
		m_Color.r = r; m_Color.g = g; m_Color.b = b;
	}
private:
	glm::vec3 m_Color;
};

class CheckerTexture : public Texture
{
public:
    CheckerTexture() {}

    CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
        : even(_even), odd(_odd)
    {
    }

    CheckerTexture(glm::vec3 c1, glm::vec3 c2)
        : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2))
    {
    }

    virtual glm::vec3 GetColor(glm::vec2 uv, const glm::vec3& point) const override
    {
        float sines = sin(10 * point.x) * sin(10 * point.y) * sin(10 * point.z);
        if (sines < 0)
            return odd->GetColor(uv, point);
        else
            return even->GetColor(uv, point);
    }

public:
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;
};

class ImageTexture : public Texture
{
public:
    const static int bytes_per_pixel = 3;

    ImageTexture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0)
    {
    }

    ImageTexture(const char* filename)
    {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data)
        {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~ImageTexture()
    {
        delete data;
    }

    virtual glm::vec3 GetColor(glm::vec2 uv, const glm::vec3& point) const override
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return glm::vec3(1, 0, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        //uv = glm::clamp(uv, glm::vec2(0.0f), glm::vec2(1.0f));
        uv[0] = glm::clamp(uv[0], 0.0f, 1.0f);
        uv[1] = 1.0f - glm::clamp(uv[1], 0.0f, 1.0f);  // Flip V to image coordinates

        int i = (int)(uv[0] * width);
        int j = (int)(uv[1] * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const float color_scale = 1.0f / 255.0f;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return glm::vec3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};