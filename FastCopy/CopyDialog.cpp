#include "pch.h"
#include "CopyDialog.h"
#if __has_include("CopyDialog.g.cpp")
#include "CopyDialog.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::FastCopy::implementation
{
    int32_t CopyDialog::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void CopyDialog::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void CopyDialog::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
