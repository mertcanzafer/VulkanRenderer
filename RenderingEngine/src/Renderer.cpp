#include "HelloTriangleApplication.h"

int main()
{
	try {
		HelloTriangleApplication().Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
}