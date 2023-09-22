// 4 december 2014
#include "uipriv_windows.hpp"

typedef std::vector<uint8_t> byteArray;

static std::map<uint8_t *, byteArray *> heap;

#ifndef LIBUI_NO_DEBUG
static std::map<byteArray *, const char *> types;
#endif

void initAlloc(void)
{
	// do nothing
}

void uninitAlloc(void)
{
	std::ostringstream oss;
	std::string ossstr;		// keep alive, just to be safe

	if (heap.size() == 0)
		return;
#ifndef LIBUI_NO_DEBUG
	for (const auto &alloc : heap)
		// note the void * cast; otherwise it'll be treated as a string
		oss << (void *) (alloc.first) << " " << types[alloc.second] << "\n";
#endif
	ossstr = oss.str();
	uiprivUserBug("Some data was leaked; either you left a uiControl lying around or there's a bug in libui itself. Leaked data:\n%s", ossstr.c_str());
}

#define rawBytes(pa) (&((*pa)[0]))

void *uiprivAlloc(size_t size, const char *type)
{
	byteArray *out;

	out = new byteArray(size, 0);
	heap[rawBytes(out)] = out;
#ifndef LIBUI_NO_DEBUG
	types[out] = type;
#endif
	return rawBytes(out);
}

void *uiprivRealloc(void *_p, size_t size, const char *type)
{
	uint8_t *p = (uint8_t *) _p;
	byteArray *arr;

	if (p == NULL)
		return uiprivAlloc(size, type);
	arr = heap[p];
	// TODO does this fill in?
	arr->resize(size, 0);
	heap.erase(p);
	heap[rawBytes(arr)] = arr;
	return rawBytes(arr);
}

void uiprivFree(void *_p)
{
	uint8_t *p = (uint8_t *) _p;

	if (p == NULL)
		uiprivImplBug("attempt to uiprivFree(NULL)");
#ifndef LIBUI_NO_DEBUG
	types.erase(heap[p]);
#endif
	delete heap[p];
	heap.erase(p);
}
