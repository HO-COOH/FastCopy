// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ProgressBarEx.xaml.h"
#if __has_include("ProgressBarEx.g.cpp")
#include "ProgressBarEx.g.cpp"
#endif
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    ProgressBarEx::ProgressBarEx()
    {
        InitializeComponent();
    }

	winrt::Microsoft::UI::Xaml::DependencyProperty ProgressBarEx::s_colorProperty = winrt::Microsoft::UI::Xaml::DependencyProperty::Register(
		L"Color",
		winrt::xaml_typename<winrt::Windows::UI::Color>(),
		winrt::xaml_typename<winrt::FastCopy::ProgressBarEx>(),
		winrt::Microsoft::UI::Xaml::PropertyMetadata{ winrt::box_value(winrt::Microsoft::UI::Colors::Green()) }
	);

	winrt::Microsoft::UI::Xaml::DependencyProperty ProgressBarEx::s_percentProperty = winrt::Microsoft::UI::Xaml::DependencyProperty::Register(
		L"Percent",
		winrt::xaml_typename<double>(),
		winrt::xaml_typename<winrt::FastCopy::ProgressBarEx>(),
		winrt::Microsoft::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0) }
	);

	void ProgressBarEx::UserControl_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(Bar());
		auto compositor = visual.Compositor();
		m_propertySet = compositor.CreatePropertySet();
		m_propertySet.InsertScalar(L"width", 50);

		m_pointLight = compositor.CreatePointLight();
		m_pointLight.Color(winrt::Microsoft::UI::Colors::White());
		m_pointLight.CoordinateSpace(visual);
		m_pointLight.Targets().Add(visual);
		m_pointLight.Offset(
			{
				(float)-Bar().ActualWidth(),
				(float)Bar().ActualHeight() / 2.0f,
				(float)30
			});

		m_animation = compositor.CreateScalarKeyFrameAnimation();
		//m_animation.InsertKeyFrame(1.0f, 2 * Bar().ActualWidth());
		m_animation.SetReferenceParameter(L"propertySet", m_propertySet);
		m_animation.InsertExpressionKeyFrame(1.0f, L"propertySet.width");
		m_animation.Duration(std::chrono::seconds{ 3 });
		m_animation.IterationBehavior(winrt::Microsoft::UI::Composition::AnimationIterationBehavior::Forever);
		m_animation.Target(L"Offset.X");

		auto intensityAnimation = compositor.CreateScalarKeyFrameAnimation();
		intensityAnimation.Target(L"Intensity");
		intensityAnimation.Duration(std::chrono::seconds{ 3 });
		intensityAnimation.IterationBehavior(winrt::Microsoft::UI::Composition::AnimationIterationBehavior::Forever);
		intensityAnimation.InsertKeyFrame(0.0, 0.0);
		intensityAnimation.InsertKeyFrame(0.2, 1.0);
		intensityAnimation.InsertKeyFrame(0.6, 1.0);
		intensityAnimation.InsertKeyFrame(1.0, 0.0);


		auto group = compositor.CreateAnimationGroup();
		group.Add(m_animation);
		group.Add(intensityAnimation);

		//m_pointLight.StartAnimation(L"Offset.X", animation);
		m_pointLight.StartAnimationGroup(group);

		m_ambientLight = compositor.CreateAmbientLight();
		m_ambientLight.Intensity(1.0);
		m_ambientLight.Color(winrt::Microsoft::UI::Colors::White());
		m_ambientLight.Targets().Add(visual);
	}

	void ProgressBarEx::Bar_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const&)
	{
		auto const width = Bar().ActualWidth();
		if (m_propertySet && width != 0 && !std::isnan(width))
			m_propertySet.InsertScalar(L"width", width);
	}
}
