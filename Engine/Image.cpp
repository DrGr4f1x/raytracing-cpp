//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Image.h"


using namespace std;
using namespace Math;


Image::Image(int width, int height)
	: m_width(width)
	, m_height(height)
{
	Init();
}


Image::Image(Image && other)
	: m_width(other.m_width)
	, m_height(other.m_height)
	, m_invWidth(other.m_invWidth)
	, m_invHeight(other.m_invHeight)
	, m_imageData(move(other.m_imageData))
{}


void Image::SetPixel(int i, int j, Vector3 color)
{
	m_imageData[i + j * m_width] = color;
}


Vector3* Image::GetData()
{
	return m_imageData.get();
}


void Image::Init()
{
	m_invWidth = 1.0f / static_cast<float>(m_width);
	m_invHeight = 1.0f / static_cast<float>(m_height);

	m_imageData = make_unique<Vector3[]>(m_width * m_height);
}


void Image::SaveAs(const char* filename)
{
	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);

	outfile << "P3\n" << m_width << " " << m_height << "\n255\n";

	for (int j = m_height - 1; j >= 0; --j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			const Vector3& color = m_imageData[i + j * m_width];
			int ir = int(255.99f * color.GetX());
			int ig = int(255.99f * color.GetY());
			int ib = int(255.99f * color.GetZ());

			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}

	outfile.close();
}