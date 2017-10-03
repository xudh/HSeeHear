#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

/* RGB: Red Green Blue 红绿蓝
 * HSB: Hue Saturation Brightness 色调 饱和度 亮度，模型图是倒棱锥，
 * b是从低锥往顶面递增的，同截面b值相等；s是截面圆心往外边递增的直到最大值，所以不同截面同s对应的长度不同
 * HSV: Hue Saturation Value 色调 饱和度 值，和HSB等价
 * HSL: Hue Saturation Lightness 色调 饱和度(和HSB的有差异) 明度(白度)

 * 参考：http://blog.csdn.net/xhhjin/article/details/7020449
 * http://www.colorizer.org/
 * http://blog.csdn.net/cy_tec/article/details/51454046
 * https://www.zhihu.com/question/22077462
 * http://www.jianshu.com/p/f03e9ac9c9ef
 */

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
		float fH = 0.0f;
		// 最大色在色环中的角度 + ((它后色值 - 它前色值) / (最大色值 - 最小色值)) * 半个段区间
		if (max == R)
		{
			fH = 0 + 60 * (G - B) / diff;
			if (fH < 0.0)	// 环形的，可能为负
				fH = 360 - fH;
		}
		else if (max == G)
			fH = 120 + 60 * (B - R) / diff;
		else// if (max == b)
			fH = 240 + 60 * (R - G) / diff;
		h = fH + 0.5f;		// 四舍五入
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

	if (h == 360)	// 360度
		h = 0;

	int HueSegId = (h / 60) % 6;					// 基准色调段
	float HueSegOff = (float)h / 60.0f - HueSegId;	// 偏移率
	float max = b;
	float min = max * (1.0f - s);
	float centerFront = max * (1.0f - HueSegOff * s);	// 中间值，在最大值对应的色环角前面的时候
	float centerBehind = max * (1.0f - (1.0f - HueSegOff) * s);

	float r = 0.0f, g = 0.0f, blue = 0.0f;
	switch (HueSegId)
	{
		case 0:
			r = max;
			g = centerBehind;
			blue = min;
			break;
		case 1:
			r = centerFront;
			g = max;
			blue = min;
			break;
		case 2:
			r = min;
			g = max;
			blue = centerBehind;
			break;
		case 3:
			r = min;
			g = centerFront;
			blue = max;
			break;
		case 4:
			r = centerBehind;
			g = min;
			blue = max;
			break;
		case 5:
			r = max;
			g = min;
			blue = centerFront;
			break;
		default:
			return -1;
	}

	*pR = r * 255.0f;
	*pG = g * 255.0f;
	*pB = blue * 255.0f;
	return 0;
}

// RGB转HSL，目前只支持8位的RGB, H(0-360), s(0.0-1.0), b(0.0-1.0)
void RGBToHSL(uint8_t r, uint8_t g, uint8_t b, unsigned int *pH, float *pS, float *pL)
{
	int R = r, G = g, B = b;
	int min = (R < G) ? (R < B ? R : B) : (G < B ? G : B);
	int max = (R > G) ? (R > B ? R : B) : (G > B ? G : B);

	unsigned int h = 0;
	int sum = min + max;
	float diff = max - min;
	if (max != min)
	{
		float fH = 0.0f;
		// 最大色在色环中的角度 + ((它后色值 - 它前色值) / (最大色值 - 最小色值)) * 半个段区间
		if (max == R)
		{
			fH = 0 + 60 * (G - B) / diff;
			if (fH < 0.0)	// 环形的，可能为负
				fH = 360 - fH;
		}
		else if (max == G)
			fH = 120 + 60 * (B - R) / diff;
		else// if (max == b)
			fH = 240 + 60 * (R - G) / diff;
		h = fH + 0.5f;		// 四舍五入
	}

	*pH = h;
	if (sum == 0 || sum == 2 * 255)
		*pS = 0.0f;
	else if (sum < 255)
		*pS = diff / sum;
	else
		*pS = diff / (2 * 255 - sum);
	*pL = (float)sum / (2 * 255);
}

static ArgTip(const char *argv0)
{
	puts("usage:");
	printf("%s RGBToHSB (0-255) (0-255) (0-255)\n", argv0);
	printf("%s HSBToRGB (0-360) (0.0-1.0) (0.0-1.0)\n", argv0);
	printf("%s RGBToHSL (0-255) (0-255) (0-255)\n", argv0);
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
		printf("HSB = %u %.2f %.2f\n", h, s, brightness);
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
	else if (strcmp(argv[1], "RGBToHSL") == 0)
	{
		if (argc != 5)
		{
			printf("usage:%s RGBToHSL (0-255) (0-255) (0-255)\n", argv[0]);
			return -1;
		}

		int r = atoi(argv[2]);
		int g = atoi(argv[3]);
		int b = atoi(argv[4]);
		if (r > 255 || g > 255 || b > 255)
		{
			printf("usage:%s RGBToHSL (0-255) (0-255) (0-255)\n", argv[0]);
			return -1;
		}
		unsigned int h = 0;
		float s = 0., l = .0;
		RGBToHSL(r, g, b, &h, &s, &l);
		printf("HSL = %u %.2f %.2f\n", h, s, l);
		return 0;
	}

	ArgTip(argv[0]);
	return -1;
}

