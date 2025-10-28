#include "pch.h"
#include "SpeedGraphData.h"

winrt::Microsoft::UI::Xaml::Media::PointCollection& SpeedGraphData::Points()
{
    if (!m_points)
        m_points = {};
    return m_points;
}

void SpeedGraphData::addInitialPointIfNeeded(uint32_t& count)
{
    if (count == 0)
    {
        m_points.Append({ 0, m_graphSize.Height });
        ++count;
    }
}

SpeedGraphData::SetSpeedResult SpeedGraphData::SetSpeed(double percent, uint64_t speed)
{
    SetSpeedResult result;

    if (m_currentMax == 0 && speed == 0)
        return result;

    //If we don't have progress on this new data point, don't show it on the graph
    if (percent <= m_currentPercent)
        return result;

    if (!m_points)
        return result;

    m_currentPercent = percent;

    auto count = m_points.Size();
    float const x = percent / 100.0 * m_graphSize.Width;
    auto const y = getY(speed);

    assert(count != 1);
    switch (count)
    {
        case 0:
            addInitialPointIfNeeded(count);
            // add first real points
            m_points.Append({ x, y });
            break;
        case 2:
            // now we can show the graph by adding this data point and the bottom point of the graph so that the polygon end at the axis
            m_points.Append({ x, y });
            m_points.Append({ x, m_graphSize.Height });
            break;
        default:
            // we modify last point (which is the bottom point of the graph) to the new data point, and add a new bottom point
            m_points.SetAt(m_points.Size() - 1, { x, y });
            m_points.Append({ x, m_graphSize.Height });
            break;
    }


    if (m_currentMax < speed)
    {
        result.newScaleRatio = static_cast<float>(m_currentMax) / speed;
        m_currentMax = speed;
    }

    if (count > 2)
        result.needAnimation = true;

    return result;
}
