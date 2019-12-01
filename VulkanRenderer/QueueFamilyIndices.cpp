#include "QueueFamilyIndices.h"

#include <optional>

bool QueueFamilyIndices::isComplete()
{
	return graphics.has_value() && presentation.has_value();
}
