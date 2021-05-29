#pragma once

// Macro to secure a class by deleting its copy constructors and assignment operators
#define DELETE_COPY_ASSIGNMENT(className)                \
	className(const className& other) = delete;          \
	className(const className&& other) = delete;           \
	className& operator=(const className& other) = delete;  \
	className& operator=(const className&& other) = delete;

// Lerps between two values a and b with factor f
#define LERP(a, b, f)		(a + f * (b - a))