
/*
* 1. 引用计数
* 2. 主动释放vtk对象
* 3. 调试vtk源码
* 4. AddObserver使用lambda，将vtk指针指向的数据置为空
*/

#define TEST4

#ifdef TEST1

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkObjectFactory.h>

#include <vector>
#include <stack>
#include <iostream>

class CustomWidget :public vtkOrientationMarkerWidget
{
public:
    static CustomWidget* New();
    vtkTypeMacro(CustomWidget, vtkOrientationMarkerWidget);

protected:
    CustomWidget()
    {
        std::cout << "construct\n";
        std::cout << "ref count:" << this->GetReferenceCount() << '\n'; //1
    }
    ~CustomWidget() override
    {
        std::cout << "destruct\n";
        std::cout << "ref count:" << this->GetReferenceCount() << '\n'; //0
    }

private:
    CustomWidget(const CustomWidget&) = delete;
    CustomWidget& operator=(const CustomWidget&) = delete;
};

vtkStandardNewMacro(CustomWidget);

int main(int, char* [])
{
    {
        std::stack<vtkSmartPointer<vtkCamera>> cameras;
        vtkNew<vtkCamera> camera1;
        cameras.emplace(camera1);
        cameras.pop();
    }

    {
        std::stack<vtkSmartPointer<CustomWidget>> widgets;
        vtkNew<CustomWidget> widget;
        widgets.emplace(widget);
        widgets.pop();
    }


    std::cout << "11111111111\n";
    {
        std::vector<vtkSmartPointer<CustomWidget>> widgets;
        vtkNew<CustomWidget> w;
        vtkSmartPointer<CustomWidget> w1 = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        widgets.emplace_back(w);
        std::cout << w->GetReferenceCount() << '\n';

        //widgets.front()->Delete();  //崩溃
        //widgets.front()->FastDelete();//崩溃
        //w->Delete();//崩溃
        //w = nullptr;//不能赋值为nullptr

        std::cout << "==========\n";

    }

    std::cout << "2222222222222\n";
    {
        std::vector<vtkSmartPointer<CustomWidget>> widgets;
        vtkSmartPointer<CustomWidget> w = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        widgets.emplace_back(w);
        std::cout << w->GetReferenceCount() << '\n';
        //widgets.front()->Delete();
        //widgets.front()->FastDelete();
        //w->Delete();

        //w = nullptr;
        widgets.front() = nullptr;
        w = nullptr;

        std::cout << "==========\n";
    }

    std::cout << "3333333333333\n";
    {

        vtkSmartPointer<CustomWidget> w = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        //widgets.front()->Delete();
        //widgets.front()->FastDelete();
        //w->Delete();

        std::cout << "==========\n";
    }

    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <iostream> 
#include <vtkObject.h> 
#include <vtkObjectFactory.h> 
#include <vtkSmartPointer.h> 

class vtkMyClass : public vtkObject 
{
public:
    vtkTypeMacro(vtkMyClass, vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent) {}
    static vtkMyClass* New();
protected:
    vtkMyClass()
    {
        std::cerr << "constructor called\n";
    }
    ~vtkMyClass()
    {
        std::cerr << "destructor called\n";
    }
};
vtkStandardNewMacro(vtkMyClass)

int main(int argc, char** argv) {
    std::cerr << __LINE__ << '\n';
    vtkSmartPointer<vtkMyClass> myObject;
    std::cerr << __LINE__ << '\n';
    myObject = vtkSmartPointer<vtkMyClass>::New();
    std::cerr << __LINE__ << '\n';
    myObject = NULL; // calls destructor 
    std::cerr << __LINE__ << '\n';
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

// 加载顶点拓扑数据等
// vtkOpenGLBufferObject.cxx bool vtkOpenGLBufferObject::UploadInternal()

// 设置状态，点的大小，线的粗细，深度测试等等
// vtkOpenGLState.cxx

// 设置uniform等
// vtkShaderProgram.cxx 

// 编译shader
// vtkShader.cxx

// FBO
// vtkOpenGLFrameBufferObject.cxx

int main()
{
    vtkNew<vtkNamedColors> colors;

    std::array<std::array<float, 3>, 8> pts = { { { { 0, 0, 0 } }, { { 1, 0, 0 } }, { { 1, 1, 0 } },
      { { 0, 1, 0 } }, { { 0, 0, 1 } }, { { 1, 0, 1 } }, { { 1, 1, 1 } }, { { 0, 1, 1 } } } };
    // The ordering of the corner points on each face.
    std::array<std::array<vtkIdType, 4>, 6> ordering = { { { { 0, 1, 2, 3 } }, { { 4, 5, 6, 7 } },
      { { 0, 1, 5, 4 } }, { { 1, 2, 6, 5 } }, { { 2, 3, 7, 6 } }, { { 3, 0, 4, 7 } } } };

    // We'll create the building blocks of polydata including data attributes.
    vtkNew<vtkPolyData> cube;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> polys;
    vtkNew<vtkFloatArray> scalars;

    // Load the point, cell, and data attributes.
    for (auto i = 0ul; i < pts.size(); ++i)
    {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i);
    }
    for (auto&& i : ordering)
    {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }

    // We now assign the pieces to the vtkPolyData.
    cube->SetPoints(points);
    cube->SetPolys(polys);
    cube->GetPointData()->SetScalars(scalars);

    // Now we'll look at it.
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cube);
    cubeMapper->SetScalarRange(cube->GetScalarRange());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    // The usual rendering stuff.
    //vtkNew<vtkCamera> camera;
    //camera->SetPosition(1, 1, 1);
    //camera->SetFocalPoint(0, 0, 0);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(cubeActor);
    //renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

    renWin->SetSize(600, 600);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}


#endif // TEST3

#ifdef TEST4

#include <vtkCubeSource.h>
#include <vtkArrowSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>

#include <iostream>

class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        std::cout << "command callback\n";
    }
};

vtkStandardNewMacro(MyCommand);


int main(int, char* [])
{
    vtkNew<vtkCubeSource> cube;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    
    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // 继承vtkCommand设置回调函数
    vtkNew<MyCommand> cb1;
    iren->AddObserver(vtkCommand::RightButtonPressEvent, cb1);

    // 用lambda设置VTK的回调函数
    vtkNew<vtkCallbackCommand> cb2;
    cb2->SetCallback([](vtkObject* caller, unsigned long, void*, void*) {std::cout << "lambda\n"; });
    iren->AddObserver(vtkCommand::LeftButtonPressEvent, cb2);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST4
