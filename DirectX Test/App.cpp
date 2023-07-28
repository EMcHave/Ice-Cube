#include <d3d11_2.h>
#include "pch.h"
#include "Common/DirectXSample.h"
#include "Common/DeviceResources.h"
#include "Common/BasicLoader.h"
#include "Common/BasicReaderWriter.h"
#include "GameClass.h"


using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;


struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
    IFrameworkView CreateView()
    {
        return *this;
    }

    // This method is called on application launch.
    void Initialize(CoreApplicationView const& applicationView)
    {
        applicationView.Activated({ this, &App::OnActivated });
        CoreApplication::Suspending({ this, &App::OnSuspending });
        CoreApplication::Resuming({ this, &App::OnResuming });
        m_deviceResources = std::make_shared<DX::DeviceResources>();
    }

    void Load(winrt::hstring const& /*entryPoint*/)
    {
        if (!m_main)
            m_main = make_self<GameClass>(m_deviceResources);
    }

    void Uninitialize()
    {
    }

    void OnActivated(CoreApplicationView const& /* applicationView */, IActivatedEventArgs const& /* args */)
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();
    }


    void Run()
    {   
        m_main->CreateDeviceDependentResources();
        m_main->CreateWindowSizeDependentResources();

        m_main->Logic()->IsRealTime(1);
        m_main->Logic()->DT(pow(10, -1));

        if (m_main->Logic()->IsRealTime())
        {
            while (true)
            {
                m_window.get().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
                m_main->Update(0);

                if (m_main->Render())
                    m_deviceResources->Present();
            }
        }
        else
        {
            float time = 2;
            int N = time / m_main->Logic()->DT();

            int dt = N / (time * 60);

            //m_window.get().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
            int begin = GetTickCount64();
            for (int n = 0; n < N; n++)
                m_main->Logic()->TimeStep();
            elapsedTime = GetTickCount64() - begin;
            
            while (true)
            {
                for (int n = 0; n < N; n += dt)
                {
                    m_window.get().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
                    m_main->Update(n);
                    if (m_main->Render())
                        m_deviceResources->Present();
                }
            }
        }
    }


    void SetWindow(CoreWindow const& window)
    {
        m_window = window;
        window.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));

        PointerVisualizationSettings visualizationSettings{ PointerVisualizationSettings::GetForCurrentView() };
        visualizationSettings.IsContactFeedbackEnabled(false);
        visualizationSettings.IsBarrelButtonFeedbackEnabled(false);

        m_deviceResources->SetWindow(window);

        window.Activated({ this, &App::OnWindowActivationChanged });

        window.SizeChanged({ this, &App::OnWindowSizeChanged });

        window.Closed({ this, &App::OnWindowClosed });

        window.VisibilityChanged({ this, &App::OnVisibilityChanged });

        DisplayInformation currentDisplayInformation{ DisplayInformation::GetForCurrentView() };

        currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });

        currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });

        DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });
    }

    winrt::fire_and_forget OnSuspending(IInspectable const& /* sender */, SuspendingEventArgs const& args)
    {
        auto lifetime = get_strong();

        SuspendingDeferral deferral = args.SuspendingOperation().GetDeferral();

        co_await winrt::resume_background();

        m_deviceResources->Trim();

        //m_main->Suspend();

        deferral.Complete();
    }

    void OnResuming(IInspectable const& /* sender */, IInspectable const& /* args */)
    {
        //m_main->Resume();
    }

    void OnVisibilityChanged(CoreWindow const& /* sender */, VisibilityChangedEventArgs const& args)
    {
        //m_main->Visibility(args.Visible());
    }

    void App::OnWindowActivationChanged(CoreWindow const& /* sender */, WindowActivatedEventArgs const& args)
    {
        //m_main->WindowActivationChanged(args.WindowActivationState());
    }

    void OnWindowSizeChanged(CoreWindow const& /* window */, WindowSizeChangedEventArgs const& args)
    {
        m_deviceResources->SetLogicalSize(args.Size());
        m_main->CreateWindowSizeDependentResources();
    }

    void OnWindowClosed(CoreWindow const& /* sender */, CoreWindowEventArgs const& /* args */)
    {
        //m_main->Close();
    }

    void OnDpiChanged(DisplayInformation const& sender, IInspectable const& /* args */)
    {
        m_deviceResources->SetDpi(sender.LogicalDpi());
        m_main->CreateWindowSizeDependentResources();
    }

    void OnOrientationChanged(DisplayInformation const& sender, IInspectable const& /* args */)
    {
        m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
        m_main->CreateWindowSizeDependentResources();
    }

    void OnDisplayContentsInvalidated(DisplayInformation const& /* sender */, IInspectable const& /* args */)
    {
        m_deviceResources->ValidateDevice();
    }

private:
    int elapsedTime;
    winrt::agile_ref<CoreWindow> m_window;
    std::shared_ptr<DX::DeviceResources> m_deviceResources;
    winrt::com_ptr<GameClass> m_main;

    winrt::com_ptr<ID3D11VertexShader>          vertexShader;
    winrt::com_ptr<ID3D11PixelShader>           pixelShader;
    winrt::com_ptr<ID3D11InputLayout>           inputLayout;

    winrt::com_ptr<ID3D11Buffer> vertexBuffer;
    winrt::com_ptr<ID3D11Buffer> indexBuffer;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(winrt::make<App>());
}