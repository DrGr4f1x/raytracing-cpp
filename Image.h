//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

class Image
{
public:
	Image(int width, int height);
	Image(Image && other);

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	float GetInvWidth() const { return m_invWidth; }
	float GetInvHeight() const { return m_invHeight; }

	void SetPixel(int i, int j, Math::Vector3 color);
	Math::Vector3* GetData();

	void SaveAs(const char* filename);

private:
	void Init();

private:
	const int m_width;
	const int m_height;
	float m_invWidth;
	float m_invHeight;

	std::unique_ptr<Math::Vector3[]> m_imageData;
};