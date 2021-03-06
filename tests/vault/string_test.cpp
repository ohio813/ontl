#include <string>
#include <nt/new.hxx>
#pragma warning(disable:4101 4189)

//template class std::basic_string<char>;


#include <vector>
#include <list>

#pragma warning(disable:4702)

namespace 
{
  template class std::vector<int>;
  template class std::list<int>;



  char* cptr()
  {
    static char s[] = "cptr";

    return s;
  }

  const char* ccptr()
  {
    return "ccptr";
  }

}


void string_test()
{
  using std::string;
  {
    std::vector<string> v1(1, string("22"));
    std::vector<string> v2;
    v2 = v1;
    string s1; s1[0] = '1';
    const string s2; s1[0] = s2[0];
  }
  return;

  char* psz = "hi";
  const char* pcsz = "ih";
  char anyhi[] = {"anyhi"};
  char hello[22] = {"0123456789\0garbage.."};

  //string zz(1);           // here: cannot convert parameter 1 from 'int' to 'const std::allocator<T> &'
  //string zzz(L"1");       // here: cannot convert parameter 1 from 'const wchar_t [2]' to 'const std::allocator<T> &'
  string z0("x");
  string z1(psz);
  string z2(pcsz);
  string z3(ccptr());
  string z4(cptr());
  string z5(anyhi);
  string z6(hello);
  //string z7(std::nullptr);// here: cannot convert parameter 1 from 'const std::nullptr_t' to 'const std::allocator<T> &'
  //string z8(0);           // here: cannot convert parameter 1 from 'int' to 'const std::allocator<T> &'
  //string z9((void*)0);    // assert_ptr: cannot convert parameter 1 from 'const void *const ' to 'const char *const '

  const signed char* sptr;
  const unsigned char* uptr;
  //string z10(sptr);       // static_assert
  //string z11(uptr);       // static_assert

  string s("x");
  s += "2";
  s += hello;
  s += psz;
  s += pcsz;
  s += anyhi;
  s += cptr();
  s += ccptr();
  s.c_str();


  std::string s2 = s + "3";
  s2.assign("0");
  s.append("4");
  s.insert(0, "0");
  s.find("2");
  s.rfind("2");
  s.compare("0");
}
