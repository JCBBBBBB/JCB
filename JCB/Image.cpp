#include "pch.h"
#include "Image.h"
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace JCB
{
	// 이미지를 파일로부터 읽어와서 width, height, channel값을 가져온다
	// 왜? 읽어와야 그 픽셀로 장난질을 할 수 있어
	void Image::ReadFromFile(const char* fileName)
	{ 
		// 이미지 파일로 불러와서
		unsigned char* image = stbi_load(fileName, &_width, &_height, &_channels, 0);

		if (image)
		{
			cout << _width << _height << _channels << endl;
		}
		else
		{
			cout << "Error" << endl;
		}

		_pixels.resize(_width * _height);
		
		// 이제 width랑 height가 있으니까 이걸로 설정
		// 현재는 unsigned char 형태로 저장이 되어 있음 -> 이거를 float 형태로 바꿔줘야 함
		// 왜? 그래픽스에서는 색상값을 0~1 범위로 다루는게 표준
		for (int i = 0; i < _width * _height; i++)
		{
			// 각각의 pixel의 r,g,b,a 값 설정
			_pixels[i].x = image[i * _channels] / 255.0f;
			_pixels[i].y = image[i * _channels + 1] / 255.0f;
			_pixels[i].z = image[i * _channels + 2] / 255.0f;
			_pixels[i].w = 1.0f;
		}

		stbi_image_free(image);
	}

	// 저장할때는 다시 unsigned char 형태로 바꿔줘야함
	void Image::WritePNG(const char* fileName)
	{
		// 현재는 pixel에 float 형식으로 rgb가 저장되어 있음
		// 이거를 다시 unsigned char로 바꾸려면

		vector<unsigned char> image(_width * _height * _channels, 0);

		for (int i = 0; i < _width * _height; i++)
		{
			image[i * _channels] = static_cast<unsigned char>(_pixels[i].x * 255.0f);
			image[i * _channels + 1] = static_cast<unsigned char>(_pixels[i].y * 255.0f);
			image[i * _channels + 2] = static_cast<unsigned char>(_pixels[i].z * 255.0f);
			image[i * _channels + 3] = static_cast<unsigned char>(_pixels[i].w * 255.0f);
		}

		stbi_write_png(fileName, _width, _height, _channels, image.data(), _width * _channels);
	}

	Vec4& Image::GetPixel(int i, int j) // 음수이면 0으로 되고 최대치 넘으면 _width-1
	{
		// TODO: 여기에 return 문을 삽입합니다.
		i = std::clamp(i, 0, _width - 1);
		j = std::clamp(j, 0, _height - 1);

		return _pixels[i + _width * j]; // 2차원 배열을 1차원으로
	}

	void Image::BoxBlur5()
	{
		vector<Vec4> pixelBuffer(_pixels.size(), { 0.f,0.f,0.f,0.f });

		for (int j = 0; j < _height; j++)
		{
			for (int i = 0; i < _width; i++)
			{
				
				Vec4 temp{ 0.f,0.f,0.f,0.f };

				// 5개의 픽셀들의 평균을 다 더해준다
				for (int num = 0; num < 5; num++)
				{
					Vec4 neighbor = GetPixel(i + num - 2, j);

					temp.x += neighbor.x; // r값
					temp.y += neighbor.y; // g값
					temp.z += neighbor.z; // b값
				}

				// 현재 픽셀에 위에서 구한 5개의 픽셀들의 평균 값으로 바꾼다
				pixelBuffer[_width * j + i].x = temp.x / 5.0f;
				pixelBuffer[_width * j + i].y = temp.y / 5.0f;
				pixelBuffer[_width * j + i].z = temp.z / 5.0f;
			}
		}

		swap(_pixels, pixelBuffer);

		// 이렇게 할 필요가 없다
		/*pixelBuffer.clear();
		pixelBuffer.resize(_pixels.size());*/

		for (int j = 0; j < _height; j++)
		{
			for (int i = 0; i < _width; i++)
			{
				Vec4 temp{ 0.f,0.f,0.f,0.f };

				// 이미 가로로는 다 평균치가 내어져있고 
				// 세로로 작업
				for (int num = 0; num < 5; num++)
				{
					Vec4 neighbor = GetPixel(i, j + num - 2);

					temp.x += neighbor.x; // r값
					temp.y += neighbor.y; // g값
					temp.z += neighbor.z; // b값
				}

				pixelBuffer[_width * j + i].x = temp.x / 5.0f;
				pixelBuffer[_width * j + i].y = temp.y / 5.0f;
				pixelBuffer[_width * j + i].z = temp.z / 5.0f;
			}
		}

		swap(_pixels, pixelBuffer);
	}
	void Image::GaussianBlur5()
	{
		vector<Vec4> pixelBuffer(_pixels.size());

		const float weights[5] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f };

		for (int j = 0; j < _height; j++)
		{
			for (int i = 0; i < _width; i++)
			{
				Vec4 temp = { 0.f,0.f,0.f,0.f };
				for (int num = 0; num < 5; num++)
				{
					//5개에 대해서 다 더해주고
					Vec4 neighbor = GetPixel(i + num - 2, j);

					// r,g,b 다 더해준다 각각
					temp.x += neighbor.x * weights[num];
					temp.y += neighbor.y * weights[num];
					temp.z += neighbor.z * weights[num];

				}

				pixelBuffer[j * _width + i].x = temp.x;
				pixelBuffer[j * _width + i].y = temp.y;
				pixelBuffer[j * _width + i].z = temp.z;
			}
		}

		swap(_pixels, pixelBuffer);

		for (int j = 0; j < _height; j++)
		{
			for (int i = 0; i < _width; i++)
			{
				Vec4 temp = { 0.f,0.f,0.f,0.f };
				for (int num = 0; num < 5; num++)
				{
					//5개에 대해서 다 더해주고
					Vec4 neighbor = GetPixel(i, j + num - 2);

					// r,g,b 다 더해준다 각각
					temp.x += neighbor.x * weights[num];
					temp.y += neighbor.y * weights[num];
					temp.z += neighbor.z * weights[num];

				}

				pixelBuffer[j * _width + i].x = temp.x;
				pixelBuffer[j * _width + i].y = temp.y;
				pixelBuffer[j * _width + i].z = temp.z;
			}
		}

		swap(_pixels, pixelBuffer);
	}
}