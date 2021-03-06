//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once


enum class PrimitiveType;


class IAccelerator
{
public:
	virtual PrimitiveType GetPrimitiveType() const = 0;

	// Intersection methods
	virtual void Intersect1(Ray& ray, Hit& hit) const = 0;

	virtual void Commit() = 0;
};