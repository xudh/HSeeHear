#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

/* RGB: Red Green Blue 红绿蓝
 * HSB: Hue Saturation Brightness 色调 饱和度 亮度
 * HSV: Hue Saturation Value 色调 饱和度 值，和HSB等价
 */

// 参考：http://blog.csdn.net/xhhjin/article/details/7020449
// http://www.colorizer.org/

// RGB转HSB，目前只支持8位的RGB, H(0-360), s(0.0-1.0), b(0.0-1.0)
void RGBToHSB(uint8_t r, uint8_t g, uint8_t b, unsigned int *pH, float *pS, float *pB)
{
	int R = r, G = g, B = b;
	int min = (R < G) ? (R < B ? R : B) : (G < B ? G : B);
	int max = (R > G) ? (R > B ? R : B) : (G > B ? G : B);

	unsigned int h = 0;
	float diff = max - min;
	if (max != min)
	{
		if (max == R)
		{
			if (G >= B)
				h = 60 * (G - B) / diff;
			else
				h = 60 * (G - B) / diff + 360;
		}
		else if (max == G)
			h = 60 * (B - R) / diff + 120;
		else// if (max == b)
			h = 60 * (R - G) / diff + 240;
	}

	*pH = h;
	*pS = (max == 0) ? 0.0f : (diff / max);
	*pB = (float)max / 255.0f;
}

// HSB转RGB，H(0-360), s(0.0-1.0), b(0.0-1.0), 目前只支持8位的RGB
int HSBToRGB(unsigned int h, float s, float b, uint8_t *pR, uint8_t *pG, uint8_t *pB)
{
	if (h > 360 || s < 0.0f || s > 1.0f || b < 0.0f || b > 1.0)
	{
		printf("%s:%d h = %u, s = %f, b = %f\n", __func__, __LINE__, h, s, b);
		return -1;
	}

	int hi = (h / 60) % 6;
	float f = (float)h / 60.0f - (float)hi;	// 使用百分制代替小数
	float p = b * (1.0f - s);
	float q = b * (1.0f - f * s);
	float t = b * (1.0f - (1.0f - f) * s);

	float r = 0.0f, g = 0.0f, blue = 0.0f;
	switch (hi)
	{
		case 0:
			r = b;
			g = t;
			blue = p;
			break;
		case 1:
			r = q;
			g = b;
			blue = p;
			break;
		case 2:
			r = p;
			g = b;
			blue = t;
			break;
		case 3:
			r = p;
			g = q;
			blue = b;
			break;
		case 4:
			r = t;
			g = p;
			blue = b;
			break;
		case 5:
			r = b;
			g = p;
			blue = q;
			break;
		default:
			return -1;
	}

	*pR = r * 255.0f;
	*pG = g * 255.0f;
	*pB = blue * 255.0f;
	return 0;
}

static ArgTip(const char *argv0)
{
	printf("usage:\n%s RGBToHSB (0-255) (0-255) (0-255)\n", argv0);
	printf("%s HSBToRGB (0-360) (0.0-1.0) (0.0-1.0)\n", argv0);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		ArgTip(argv[0]);
		return -1;
	}

	if (strcmp(argv[1], "RGBToHSB") == 0)
	{
		if (argc != 5)
		{
			printf("usage:%s RGBToHSB (0-255) (0-255) (0-255)\n", argv[0]);
			return -1;
		}

		int r = atoi(argv[2]);
		int g = atoi(argv[3]);
		int b = atoi(argv[4]);
		if (r > 255 || g > 255 || b > 255)
		{
			printf("usage:%s RGBToHSB (0-255) (0-255) (0-255)\n", argv[0]);
			return -1;
		}
		unsigned int h = 0;
		float s = 0., brightness = .0;
		RGBToHSB(r, g, b, &h, &s, &brightness);
		printf("HSB = %u %0.2f %0.2f\n", h, s, brightness);
		return 0;
	}
	else if (strcmp(argv[1], "HSBToRGB") == 0)
	{
		if (argc != 5)
		{
			printf("usage:%s HSBToRGB (0-360) (0.0-1.0) (0.0-1.0)\n", argv[0]);
			return -1;
		}

		int h = atoi(argv[2]);
		float s = atof(argv[3]);
		float b = atof(argv[4]);
		if (h < 0 || h > 360 || s < 0.0f || s > 1.0f || b < 0.0f || b > 1.0f)
		{
			printf("usage:%s HSBToRGB (0-360) (0.0-1.0) (0.0-1.0)\n", argv[0]);
			return -1;
		}
		uint8_t r = 0, g = 0, blue = 0;
		if (HSBToRGB(h, s, b, &r, &g, &blue) != 0)
			return -1;
		else
		{
			printf("RGB = %"PRIu8" %"PRIu8" %"PRIu8"\n", r, g, blue);
			return 0;
		}
	}

	ArgTip(argv[0]);
	return -1;
}

