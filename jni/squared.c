#include "squared.h";

JNIEXPORT jint JNICALL Java_org_edwards_1research_demo_jni_SquaredWrapper_squared
  (JNIEnv * je, jclass jc, jint base)
{
        return (base*base);
}
