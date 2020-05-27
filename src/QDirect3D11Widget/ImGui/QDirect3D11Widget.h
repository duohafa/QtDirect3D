/*
 *
 */

#pragma once

#include <stdexcept>

#include <QWidget>
#include <QTimer>

#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class QDirect3D11Widget : public QWidget
{
    Q_OBJECT

public:
    QDirect3D11Widget(QWidget * parent);
    ~QDirect3D11Widget();

    void release();
    void resetEnvironment();

private:
    bool init();

    void beginScene();
    void endScene();

    void tick();
    void render();
    void uiRender();

// Qt Events
private:
    bool event(QEvent * event) override;
    void showEvent(QShowEvent * event) override;
    QPaintEngine * paintEngine() const override;
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#if QT_VERSION >= 0x050000
    bool nativeEvent(const QByteArray & eventType, void * message, long * result) override;
#else
    bool winEvent(MSG * message, long * result) override;
#endif


signals:
    void deviceInitialized(bool success);

    void eventHandled();
    void widgetResized();

    void ticked();
    void rendered();
    void uiRendered();


private slots:
    void onFrame();
    void onReset();

// Getters / Setters
public:
    HWND const & nativeHandle() const { return m_hWnd; }

    ID3D11Device * device() const { return m_pDevice; }
    ID3D11DeviceContext * deviceContext() { return m_pDeviceContext; }
    IDXGISwapChain * swapChain() { return m_pSwapChain; }
    ID3D11RenderTargetView* TargetView() const { return m_pRTView; }

    bool renderActive() const { return m_bRenderActive; }
    void setRenderActive(bool active) { m_bRenderActive = active; }

    DirectX::XMVECTORF32 * BackColor() { return &m_BackColor; }


private:
    ID3D11Device*            m_pDevice;
    ID3D11DeviceContext*     m_pDeviceContext;
    IDXGISwapChain*          m_pSwapChain;
    ID3D11RenderTargetView*  m_pRTView;


    QTimer                    m_qTimer;

    HWND                      m_hWnd;
    bool                      m_bDeviceInitialized;

    bool                      m_bRenderActive;

    DirectX::XMVECTORF32      m_BackColor;

};


// ############################################################################
// ############################## Utils #######################################
// ############################################################################
#define ReleaseObject(object) if((object) != nullptr) { object->Release(); object = nullptr; }
#define ReleaseHandle(object) if((object) != nullptr) { CloseHandle(object); object = nullptr; }


inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
    HRESULT Error() const { return m_hr; }
private:
    const HRESULT m_hr;
};

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

#define DXCall(func) ThrowIfFailed(func)

