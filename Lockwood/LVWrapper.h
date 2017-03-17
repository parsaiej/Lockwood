#pragma once

#include <functional>
#include <vulkan\vulkan.h>

//Couples vulkan handles to their respective destroyer functions, much like a smart pointer.
//This class was originally written by Alexander Overoorde's Vulkan tutorial.
//https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code
template <typename T>
class LVWrapper {
public:
	LVWrapper() : LVWrapper([](T, VkAllocationCallbacks*) {}) {}
	LVWrapper(std::function<void(T, VkAllocationCallbacks*)> _deleteFunc) {
		deleter = [=](T obj) { _deleteFunc(obj, nullptr); };
	}

	LVWrapper(const LVWrapper<VkDevice> &_device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> _deleteFunc) {
		deleter = [&_device, _deleteFunc](T obj) { _deleteFunc(_device, obj, nullptr); };
	}

	LVWrapper(const LVWrapper<VkInstance> &_instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> _deleteFunc) {
		deleter = [&_instance, _deleteFunc](T obj) { _deleteFunc(_instance, obj, nullptr); };
	}

	~LVWrapper() { CleanUp(); }

	const T* operator &() const { return &object; }

	operator T() const { return object;	}

	void operator=(T _rhs) {
		if (_rhs != object) {
			CleanUp();
			object = _rhs;
		}
	}

	template<typename V>
	bool operator==(V _rhs) { return object == T(_rhs); }

	T* Replace() {
		CleanUp();
		return &object;
	}

	const T& Get() const {
		return object;
	}

private:
	T object{ VK_NULL_HANDLE };
	std::function<void(T)> deleter;

	void CleanUp() {
		if (object != VK_NULL_HANDLE) {
			deleter(object);
		}
		object = VK_NULL_HANDLE;
	}
};