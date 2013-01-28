// Default empty project template
import bb.cascades 1.0
import TestRoundProgress 1.0

// creates one page with a label
Page {
    Container {
        layout: DockLayout {}
        ImageView {
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            imageSource: "asset:///images/background.jpg"
        }
        RoundProgress {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            value: 90                                                // 1/4 of the image, because by default maxValue=360
            imageSource: "asset:///images/outer_progress_bar.png"    // Source of the image. Only local assets are supported now
        }
    }
}

