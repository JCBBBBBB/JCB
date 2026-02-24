#pragma once

namespace JCB
{
	class Image
	{
	public:
		void ReadFromFile(const char* fileName);
		void WritePNG(const char* fileName);
		Vec4& GetPixel(int i, int j);
		void BoxBlur5();
		void GaussianBlur5();

	public:
		int _width = 0, _height = 0, _channels = 0;
		std::vector<Vec4> _pixels;
	};

}