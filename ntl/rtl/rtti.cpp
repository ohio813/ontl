#include "../stlx/cstddef.hxx"
#include "../stlx/typeinfo.hxx"

#include "../pe/image.hxx"
#include "../nt/exception.hxx"
#include "../nt/status.hxx"

#ifdef NTL__SUBSYSTEM_KM
# include "../km/new.hxx"
#else
# include "../nt/new.hxx"
# include "../nt/peb.hxx"
#endif

#ifdef _MSC_VER
#pragma warning(disable:4510 4512 4610 4100)
#ifdef __ICL
#pragma warning(disable:69)
#endif
#endif

#if !STLX__USE_RTTI
#pragma message("This file needed only for ENABLED RTTI (see /GR).")
#endif

#pragma region typeinfo::name implementation

#ifndef CRT_UNDNAME
#define CRT_UNDNAME
#endif

namespace ntl { namespace __
{
#ifndef CRT_UNDNAME
  typedef size_t __stdcall undname_t(const char* signame, char* name, size_t length, uint32_t flags);
#else
  typedef void* __cdecl alloc_t(size_t);
  typedef void  __cdecl free_t(void*);
  typedef char* __cdecl undname_t(char* name, const char* signame, int max_size, alloc_t*, free_t*, uint16_t flags);

  void* __cdecl alloc_f(size_t cb) { return new char[cb]; }
  void  __cdecl free_f(void* p) { delete[] (char*)p; }
#endif

  static undname_t* undname = 0;

  bool init_undname()
  {
    typedef pe::image* __stdcall dlopen_t(const char*);
    nt::peb::find_dll fd;
    const pe::image* kernel = fd("kernel32.dll");
    dlopen_t* dlopen = kernel->find_export<dlopen_t*>("LoadLibraryA");
#ifdef CRT_UNDNAME
    const char *dll = "msvcrt.dll", *func = "__unDName";
#else
    const char *dll = "imagehlp.dll", *func = "UnDecorateSymbolName";
#endif
    const pe::image* crt = dlopen(dll);
    if(crt)
      undname = crt->find_export<undname_t*>(func);
    return undname != 0;
  }
}

const char* undname(const char* signame)
{
  if(!__::undname)
    if(!__::init_undname())
      return signame;

  static const uint16_t flags = 0x2000; /* no_arguments */

#ifndef CRT_UNDNAME
  char buf[4096];
  size_t len = __::undname(signame+1, buf, sizeof(buf), flags);
  if(!len)
    return signame;
  return std::strdup(buf);
#else
  return __::undname(0, signame+1, 0, __::alloc_f, __::free_f, flags);
#endif
}
} // namespace ntl

//////////////////////////////////////////////////////////////////////////
#ifdef _NTL_DEMANGLE
const char* std::type_info::name() const __ntl_nothrow
{
  if(!data)
    data = const_cast<char*>(ntl::undname(mname()));
  return reinterpret_cast<const char*>(data);
}
#endif

#ifdef __ICL
::type_info::~type_info()
{

}
#endif
#pragma endregion


#pragma region dynamic_cast implementation

//#if STLX__USE_RTTI

namespace ntl { namespace cxxruntime
{
  #pragma pack(push, ehdata, 4)

  typedef ntl::nt::exception::rva_t rva_t;

  /// pointer to member descriptor
  struct pmd // _PMD
  {
    rva_t   member_offset;
    rva_t   vbtable_offset; // -1 if not a virtual base
    rva_t   vdisp_offset;   // offset to the displacement value inside the vbtable

    template<class T>
    T* operator()(T* obj) const
    {
      ptrdiff_t off = 0;
      if(static_cast<int>(vbtable_offset) >= 0){
        // *(ptrdiff_t*)((char*)*(ptrdiff_t*)((char*)pThis + RetOff) + pmd.vdisp);
        off = vbtable_offset + *reinterpret_cast<const ptrdiff_t*>(*reinterpret_cast<const ptrdiff_t*>(ntl::padd(obj, vbtable_offset)) + vdisp_offset);
      }
      off += member_offset;
      return ntl::padd(obj, off);
    }
  };

  typedef void generic_function_t();

  #pragma pack(push, _TypeDescriptor, 8)
  struct typeinfo
  {
    const void* vptr;
    mutable void* spare;
  };
  static_assert(sizeof(typeinfo) == sizeof(std::type_info), "invalid cxxruntime::typeinfo");
  #pragma pack(pop, _TypeDescriptor)
  #pragma pack(pop, ehdata)

  #pragma pack(push, rttidata, 4)

  struct class_hierarchy;

  union typeinfos
  {
    const typeinfo& rawtype;
    const std::type_info& type;
  private:
    typeinfos& operator=(const typeinfos&); // make icl happy
  };

  inline bool operator==(const typeinfo& l, const typeinfo& r)
  {
    return &l == &r || reinterpret_cast<const std::type_info&>(l) == reinterpret_cast<const std::type_info&>(r);
  }

  struct base_class
  {
#ifndef _M_X64
    typeinfos   type;
#else
    rva_t       type;
#endif
    uint32_t    bases;
    pmd         thiscast;

    //uint32_t  attributes:
    uint32_t    notvisible:1;
    uint32_t    ambiguous:1;
    uint32_t    privcomp:1;
    uint32_t    privbase:1;
    uint32_t    vbofcontobj:1;
    uint32_t    nonpolymorphic:1;
  };

  struct base_class2: base_class
  {
#ifndef _M_X64
    const class_hierarchy* hierarchy;
#else
    rva_t                  hierarchy;
#endif
  };

  struct base_class_array
  {
#ifndef _M_X64
    const base_class2* bases[1];
#else
    rva_t              bases[1];
#endif
  };

  struct throwbase
  {
#ifndef _M_X64
    throwbase(const void*)
    {}

    template<typename T>
    T va(const void* p) const { return (T)p; }

    template<typename T>
    T va(const typeinfos& ti) const { return (T)&ti; }
#else
    template<typename T>
    T va(uintptr_t rva) const
    {
      return image->va<T>(rva);
    }

    uintptr_t va(uintptr_t rva) const
    {
      return image->va(rva);
    }

    explicit throwbase(const pe::image* image)
      :image(image)
    {}
    const pe::image* image;
#endif
  };

  struct class_hierarchy
  {
    uint32_t    signature;
    //uint32_t  attributes;
    uint32_t    ismultiple:1;
    uint32_t    isvirtual:1;
    uint32_t    isambiguous:1;
    uint32_t    bases;
#ifndef _M_X64
    const base_class_array* classes;
#else
    rva_t       classes;
#endif
  };

  struct object_locator
  {
    uint32_t  signature;
    int32_t   offset;
    int32_t   cdoffset;
#ifndef _M_X64
    typeinfos type;
    const class_hierarchy* hierarchy;
#else
    rva_t     type;
    rva_t     hierarchy;
#endif
    static const object_locator& instance(const void* object)
    {
      return *reinterpret_cast<const object_locator*>( ((*((void***)object))[-1]) );
    }

    const void* find_object(const void* object) const
    {
      intptr_t complete = ntl::padd<intptr_t>(object, -offset);
      if(cdoffset)
#ifndef _M_X64
        complete +=
#else
        complete -=
#endif
        *reinterpret_cast<const rva_t*>( ntl::padd(object, -cdoffset) );
      return reinterpret_cast<const void*>(complete);
    }

    const base_class2* find_instance(const void* complete, ptrdiff_t ptrdelta, const typeinfo& srctype, const typeinfo& desttype, const throwbase* imagebase = 0) const
    {
#ifdef _M_X64
      const class_hierarchy* hierarchy = imagebase->va<const class_hierarchy*>(this->hierarchy);
#else
      const class_hierarchy* hierarchy = this->hierarchy;
#endif

      if(!hierarchy->ismultiple)
        return find_single_instance(srctype, desttype, imagebase);
      else if(!hierarchy->isvirtual)
        return find_multiple_instance(complete, ptrdelta, srctype, desttype, imagebase);
      else
        return find_virtual_instance(complete, ptrdelta, srctype, desttype, imagebase);
    }

  protected:
    const base_class2* find_single_instance(const typeinfo& srctype, const typeinfo& desttype, const throwbase* imagebase) const
    {
#ifndef _M_X64
      const base_class2* base, * const * ptr = hierarchy->classes->bases;
      for(unsigned i = 0; i < hierarchy->bases; i++, ptr++){
        base = *ptr;
        if(base->type.rawtype == desttype){
          // assume that no more than one base classes are exists,
          // i.e. only one base object exists, which type equals to the target type, and it [base object] can be private,
          // so we can't give access to it.
          if(!base->notvisible)
            return base;
          break;
        }
      }
#else
      const class_hierarchy* hierarchy = imagebase->va<const class_hierarchy*>(this->hierarchy);
      const base_class_array* bases = imagebase->va<const base_class_array*>( hierarchy->classes );
      for(unsigned i = 0; i < hierarchy->bases; i++){
        const base_class2* base = imagebase->va<const base_class2*>(bases->bases[i]);
        const typeinfo* type = imagebase->va<const typeinfo*>(base->type);
        if(*type == desttype){
          // on x64 behaviour should be such as find_multiple_instance
          for(unsigned j = i + 1; j < hierarchy->bases; j++){
            const base_class2* base2 = imagebase->va<const base_class2*>(bases->bases[j]);
            if(base2->privcomp)
              break;
            type = imagebase->va<const typeinfo*>(base2->type);
            if(*type == srctype)
              return base;
          }
          break;
        }
      }
#endif
      return nullptr;
    }

    const base_class2* find_multiple_instance(const void* complete, ptrdiff_t ptrdelta, const typeinfo& srctype, const typeinfo& desttype, const throwbase* imagebase) const
    {
      const class_hierarchy* hierarchy = imagebase->va<const class_hierarchy*>(this->hierarchy);
      const base_class_array* bases = imagebase->va<const base_class_array*>( hierarchy->classes );

      const base_class2 *prevBase = nullptr, *base2 = nullptr;

      for(unsigned i = 0, prevNum = 0, previ = unsigned(-1); i < hierarchy->bases; i++){
        
        const base_class2* base = imagebase->va<const base_class2*>(bases->bases[i]);
        const typeinfo* type = 
#ifdef _M_X64
        imagebase->va<const typeinfo*>(base->type);
#else
          &base->type.rawtype;
#endif

        // check dest
        if(i - previ > prevNum && *type == desttype){
          if(!prevBase){
            prevNum = base->bases;
            base2 = base;
            previ = i;
          }else{
            // check Prev
            if(base->notvisible || base->ambiguous)
              return nullptr;
            return base;
          }
        }
        // check src
        if(!(*type == srctype))
          continue;

        static const int* nil = 0;
        if(base->thiscast(nil)-nil != ptrdelta)
          continue;

        if(!base2){
          prevBase = base;
          continue;
        }

        // base found
        if(i - previ > prevNum){
          if(base2->notvisible || base2->ambiguous)
            return nullptr;
        }
        if(!base2->nonpolymorphic){
          if(previ != 0)
            return base2;
          if(base2->notvisible)
            return nullptr;
          return base2;
        }else{
          const class_hierarchy* hierarchy = imagebase->va<const class_hierarchy*>(base2->hierarchy);
          const base_class_array* bases = imagebase->va<const base_class_array*>( hierarchy->classes );
          const base_class2* daddy = imagebase->va<const base_class2*>(bases->bases[i]);
          return daddy->notvisible ? nullptr : base2;
        }
      }
      return nullptr;
    }
    const base_class2* find_virtual_instance(const void* complete, ptrdiff_t ptrdelta, const typeinfo& srctype, const typeinfo& desttype, const throwbase* imagebase) const
    {
      __debugbreak();
      return 0;
    }
  };

  struct object_locator2: object_locator
  {
#ifndef _M_X64
    const object_locator2* self;
#else
    int32_t self;
#endif

    static const object_locator2& instance(const void* object)
    {
      return *reinterpret_cast<const object_locator2*>( ((*((void***)object))[-1]) );
    }

    void validate() const
    {
      int reader = 0;
      for(const char* p = reinterpret_cast<const char*>(this);
        p < reinterpret_cast<const char*>(this) + sizeof(*this);
        p++)
      {
        reader += *p;
      }
    }
  };

  #pragma pack(pop, rttidata)



}} // namespaces

using namespace ntl;

namespace {

  const void* RTDynamicCast(const void* object, int32_t vfdelta, const ntl::cxxruntime::typeinfo& srctype, const ntl::cxxruntime::typeinfo& desttype, bool isreference)
  {
    using namespace ntl::cxxruntime;
    __try {
      const object_locator2& locator = object_locator2::instance(object);
      locator.validate();

      const void* complete = locator.find_object(object);

      const pe::image* imagebase = nullptr;
      #ifdef _M_X64
      imagebase = !locator.signature 
        ? pe::image::base_from(&locator)
        : ntl::padd<const pe::image*>(&locator, -locator.self);
      #endif

      throwbase module_base(imagebase);

      // adjust object ptr by vptr diplacement
      object = ntl::padd(object, -vfdelta);
      const base_class2* base = locator.find_instance(complete, (const char*)object - (const char*)complete, srctype, desttype, &module_base);

      const void* result;
      if(base)
        result = base->thiscast(complete);
      else {
        result = nullptr;
        if(isreference)
          __ntl_throw(std::bad_cast(/*"Bad dynamic_cast<>"*/));
      }
      return result;
    }
    __except(exception_code == nt::status::access_violation ? exception_execute_handler : exception_continue_search){
      __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
    #if STLX__USE_EXCEPTIONS == 0
      return nullptr;
    #endif
    }
  }
} // namespace


///\note throw(...) is to avoid error C2382: redefinition; different exception specifications
/// as this is predefined C++ type
extern "C" void* __cdecl __RTCastToVoid (void* object) throw(...)
{
  if(!object)
    return nullptr;

  __try {
    const cxxruntime::object_locator2& locator = cxxruntime::object_locator2::instance(object);
    locator.validate();
    return (void*)locator.find_object(object);
  }
  __except(exception_code == nt::status::access_violation ? exception_execute_handler : exception_continue_search){
    __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
    #if STLX__USE_EXCEPTIONS == 0
      return nullptr;
    #endif
  }
}

extern "C" void* __cdecl __RTtypeid(void* object) throw(...)
{
  if(!object){
    __ntl_throw(std::bad_typeid(/*"A typeid of nullptr pointer attempted"*/));
    #if STLX__USE_EXCEPTIONS == 0
      return nullptr;
    #endif
  }
  __try{
    const cxxruntime::object_locator2& locator = cxxruntime::object_locator2::instance(object);
    locator.validate();
#ifndef _M_X64
    return (void*)&locator.type.type;
#else
    const pe::image* pe = nullptr;
    if(!locator.type || !(pe = pe::image::base_from(&locator), pe)){
      pe = pe::image::this_module();
    //  __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
    //#if STLX__USE_EXCEPTIONS == 0
    //  return nullptr;
    //#endif
    }
    return pe->va<void*>(locator.type);
#endif
  }
  __except(exception_code == nt::status::access_violation ? exception_execute_handler : exception_continue_search){
    __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
    #if STLX__USE_EXCEPTIONS == 0
      return nullptr;
    #endif
  }
}

#include "../stlx/stdexcept.hxx"

extern "C" void* __cdecl __RTDynamicCast(void* object, int32_t vfdelta, void* srctype, void* desttype, int isreference) throw(...)
{
  if(!object)
    return nullptr;

  return (void*)RTDynamicCast(object, vfdelta, *reinterpret_cast<ntl::cxxruntime::typeinfo*>(srctype), *reinterpret_cast<ntl::cxxruntime::typeinfo*>(desttype), isreference != 0);
}

//#endif // rtti
#pragma endregion

