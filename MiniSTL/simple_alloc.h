#ifndef _SIMPLE_ALLOC_H
#define _SIMPLE_ALLOC_H


namespace MiniSTL {

    template<class T,class Alloc>
    class simple_alloc {
    public:
        // ������ת���ã����ô��ݸ�������(��һ����ڶ���)����һ���װ��ʹ _Alloc �߱���׼�ӿ�
        static T* allocate(size_t n) {
            if (n == 0)
                return 0;
            return (T*)Alloc::allocate(n * sizeof(T));
        }

        //��Ϊallocate��allocΪ��ģ���࣬����alloc��allocate�޸ó�Ա����
        //������simple_alloc��Ϊ�ӿڣ�����T �γ�  static T* allocate()
        static T* allocate() {
            return (T*)Alloc::allocate(sizeof(T));
        }

        //һ��������ֱ��free ������n�޹�
        //��������������nֵ��ѡ���Ƿ����һ���������������ö���������nѡ���С���ʵ���������
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
