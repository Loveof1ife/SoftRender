//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"
#include <thread>
#include <mutex>

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }
int g_complateTotals = 0;
std::mutex g_mutex;

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void render_thread(std::vector<Vector3f>& fbuffer, const Scene& scene,int spp, int y0, int y1)
{
    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    for (int i = y0; i < y1; i++)
    {
        for (int j = 0; j < scene.width; j++)
        {
            int index = i * scene.width + j;
            for (int k = 0; k < spp; k++)
            {
                float x = get_random_float();
                float y = get_random_float();
                float _x = (2 * (j + x) / (float)scene.width - 1) *
                           imageAspectRatio * scale;
                float _y = (1 - 2 * (i + y) / (float)scene.height) * scale;
                Vector3f dir = normalize(Vector3f(-_x, _y, 1));
                Ray ray = Ray(eye_pos, dir);
                fbuffer[index] += scene.castRay(ray, 0) / spp;
            }
        }
        g_mutex.lock();
        g_complateTotals++;
        UpdateProgress(g_complateTotals / (float)scene.height);
        g_mutex.unlock();
    }

}

void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);

    int m = 0;
    g_complateTotals = 0;
    // change the spp value to change sample ammount
    int spp = 16;

    int numThreads = std::thread::hardware_concurrency();
    int lines = scene.height / numThreads + 1;
    std::vector<std::thread> workers;

    for (int i = 0; i < numThreads; i++)
    {
        int y0 = i * lines;
        int y1 = std::min(y0 + lines,scene.height);
        std::cout << "id:" <<i << " " << y0 << "=>" << y1 << std::endl;
        //为什么要使用 std::ref?
        //在 C++ 中，尽管 render_thread 函数声明参数为引用，但是 std::thread 构造函数的工作方式导致了一种特殊的行为。
        // std::thread 的构造函数通过值复制其所接受的参数到内部存储，以便于安全地在新线程中使用这些参数。这意味着即使你的目标函数 render_thread 期望引用参数，std::thread 构造器仍然会首先尝试复制这些参数。如果不使用 std::ref，尽管函数声明要求引用，编译器会尝试对这些参数进行复制，然后将引用传递给复制后的对象。
        //何时使用 std::ref
        //当你需要确保：
        //引用传递：避免数据复制，尤其是对于大对象或者需要多个线程共享数据的场景。
        //数据一致性：多个线程操作同一个数据实例，而非各自操作数据的拷贝。
        workers.push_back(std::thread(render_thread, std::ref(framebuffer), std::ref(scene), spp, y0, y1));
    }




    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
