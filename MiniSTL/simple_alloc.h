#ifndef _SIMPLE_ALLOC_H
#define _SIMPLE_ALLOC_H


namespace MiniSTL {

    template<class T,class Alloc>
    class simple_alloc {
    public:
        // 单纯地转调用，调用传递给配置器(第一级或第二级)；多一层包装，使 _Alloc 具备标准接口
        static T* allocate(size_t n) {
            if (n == 0)
                return 0;
            return (T*)Alloc::allocate(n * sizeof(T));
        }

        //因为allocate和alloc为非模板类，所以alloc和allocate无该成员函数
        //但是以simple_alloc作为接口，利用T 形成  static T* allocate()
        static T* allocate() {
            return (T*)Alloc::allocate(sizeof(T));
        }

        //一集配置器直接free 所以与n无关
        //二级配置器根据n值，选择是否调用一级配置器，若调用二级，根据n选择大小合适的区块链表
        static void deallocate(T* p,size_t n) {
            if(n!=0)
                Alloc::deallocate(p,sizeof(T)*n);
        }
        static void deallocate(T* p) {
                Alloc::deallocate(p, sizeof(T));
        }
    };
}

#endif 
