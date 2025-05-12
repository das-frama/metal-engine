#include "core/input.h"
#include "platform/platform.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <CoreGraphics/CGGeometry.h>
#include <CoreGraphics/CGRemoteOperation.h>
#include <Foundation/Foundation.h>
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

struct Platform {
    NSWindow *window;
    NSApplication *app;
    NSView *view;
    CAMetalLayer *metal_layer;

    NSCondition *condition;
    id display_link;
    CVDisplayLinkRef display_link_dep;
    bool frame_ready;
    id display_callback;

    bool *should_close_window;
    bool initialized;
};

// –––––––––––––––––––----–
// ----- AppDelegate ------
// –––––––––––––––––––----–
@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
- (bool)applicationShouldAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (void)applicationDidResignActive:(NSNotification *)notification {
    // Освобождаем курсор при потере фокуса
    CGAssociateMouseAndMouseCursorPosition(true); // Включаем связь курсора с мышью
    [NSCursor unhide];                            // Показываем курсор
    printf("Application lost focus, cursor released.\n");
}
- (void)applicationDidBecomeActive:(NSNotification *)notification {
    // Захватываем курсор при восстановлении фокуса
    CGAssociateMouseAndMouseCursorPosition(false); // Отключаем связь курсора с мышью
    [NSCursor hide];                               // Скрываем курсор
    printf("Application gained focus, cursor captured.\n");
}
@end

// ––––––––––––––––––––
// ----- MTKView ------
// ––––––––––––––––––––
@interface ContentView : MTKView {
    NSWindow *initWindow;
    CGPoint lastMousePosition;
}
- (instancetype)initWithWindow:(NSWindow *)window;
@end

@implementation ContentView
- (instancetype)initWithWindow:(NSWindow *)window {
    self = [super initWithFrame:window.contentView.frame device:MTLCreateSystemDefaultDevice()];
    if (self != nil) {
        self->initWindow = window;
        self->lastMousePosition = CGPointZero;
        [self.window setAcceptsMouseMovedEvents:YES];
        CGAssociateMouseAndMouseCursorPosition(false);
    }
    return self;
}
- (BOOL)acceptsFirstResponder {
    return YES;
}
- (void)keyDown:(NSEvent *)event {
    NSString *chars = [event charactersIgnoringModifiers];
    unichar key = [chars characterAtIndex:0];

    input_process_key(key, true);
}
- (void)keyUp:(NSEvent *)event {
    NSString *chars = [event charactersIgnoringModifiers];
    unichar key = [chars characterAtIndex:0];

    input_process_key(key, false);
}
- (void)mouseMoved:(NSEvent *)event {
    CAMetalLayer *layer = (CAMetalLayer *)event.window.contentView.layer;
    CGRect window_rect = layer.frame;
    // s16 x = pos.x * layer.contentsScale;
    // s16 y = window_rect.size.height - (pos.y * layer.contentsScale);
    s16 x = [event deltaX];
    s16 y = [event deltaY];

    input_process_mouse_move(x, y);

    NSPoint center = NSMakePoint(NSMidX(window_rect), NSMidY(window_rect));

    NSPoint screen_point = [event.window.contentView convertPoint:center toView:nil];
    screen_point = [event.window convertPointToScreen:screen_point];

    // CGAssociateMouseAndMouseCursorPosition(true);
    CGWarpMouseCursorPosition(CGPointMake(screen_point.x, screen_point.y));
    // CGAssociateMouseAndMouseCursorPosition(false);
}

- (void)dealloc {
    // [NSCursor unhide];
    CGAssociateMouseAndMouseCursorPosition(true);
}
@end

// PlatformCallback.
@interface PlatformCallback : NSObject
@property Platform *platform;
- (instancetype)init_with_platform:(Platform *)p;
- (void)on_display_link:(id)sender;
@end

@implementation PlatformCallback
- (instancetype)init_with_platform:(Platform *)p {
    printf("init with platform\n");
    self = [super init];
    self.platform = p;
    return self;
}
- (void)on_display_link:(id)sender {
    printf("on display link\n");
    Platform *p = self.platform;
    [p->condition lock];
    p->frame_ready = true;
    [p->condition signal];
    [p->condition unlock];
}
@end

CVReturn on_display_callback_dep(CVDisplayLinkRef displayLink, const CVTimeStamp *in, const CVTimeStamp *out,
                                 unsigned long long flagsIn, unsigned long long *flagsOut, void *context) {
    Platform *p = (Platform *)context;
    [p->condition lock];
    p->frame_ready = true;
    [p->condition signal];
    [p->condition unlock];
    return kCVReturnSuccess;
}

Platform *platform_init(int width, int height, const char *title, bool *should_close_window) {
    [NSApplication sharedApplication];

    // App Delegate.
    AppDelegate *delegate = [AppDelegate new];
    [NSApp setDelegate:delegate];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSRect screen_rect = [[NSScreen mainScreen] frame];
    NSRect frame =
        NSMakeRect(NSMidX(screen_rect) - (f32)width / 2, NSMidY(screen_rect) - (f32)height / 2, width, height);
    NSWindow *window = [[NSWindow alloc]
        initWithContentRect:frame
                  // styleMask:(NSWindowStyleMaskBorderless)
                  styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [window makeKeyAndOrderFront:nil];
    [window setTitle:[NSString stringWithUTF8String:title]];

    // Создание структуры платформы.
    Platform *p = calloc(1, sizeof(Platform));
    p->app = NSApp;
    p->window = window;
    p->should_close_window = should_close_window;

    // Создание MTKView.
    p->view = [[ContentView alloc] initWithWindow:p->window];
    [p->window setContentView:p->view];
    [p->window makeFirstResponder:p->view];

    // Создание Metal Layer.
    CAMetalLayer *metal_layer = [CAMetalLayer layer];
    metal_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metal_layer.framebufferOnly = YES;
    metal_layer.frame = frame;
    metal_layer.drawableSize = CGSizeMake(width, height);
    p->metal_layer = metal_layer;
    [p->window.contentView setLayer:p->metal_layer];
    [p->window.contentView setWantsLayer:YES];

    // Создание Tracking Area для отслеживания движения мыши.
    NSTrackingArea *area = [[NSTrackingArea alloc]
        initWithRect:[p->view bounds]
             options:(NSTrackingMouseMoved | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect)
               owner:p->view
            userInfo:nil];
    [p->view addTrackingArea:area];
    // [NSCursor hide];

    // Настройка CVDisplayLink.
    CVDisplayLinkCreateWithActiveCGDisplays(&p->display_link_dep);
    CVDisplayLinkSetOutputCallback(p->display_link_dep, on_display_callback_dep, p);
    p->condition = [NSCondition new];
    p->frame_ready = false;

    p->initialized = true;
    return p;
}

void platform_start_rendering(Platform *p) {
    CVDisplayLinkStart(p->display_link_dep);
}

void platform_poll_events(Platform *p) {
    @autoreleasepool {
        NSEvent *event;
        while ((event = [p->app nextEventMatchingMask:NSEventMaskAny
                                            untilDate:nil
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES])) {
            [p->app sendEvent:event];
            [p->app updateWindows];

            if (event.type == NSEventTypeKeyDown && event.keyCode == 53) { // ESC
                *p->should_close_window = true;
            }
        }
    }
}

bool platform_wait_frame(Platform *p) {
    if (!p->initialized) {
        printf("Platform not initialized\n");
        return false;
    }

    [p->condition lock];
    while (!p->frame_ready) {
        [p->condition wait];
    }
    p->frame_ready = false;
    [p->condition unlock];

    return true;
}

void platform_destroy(Platform *p) {
    [p->display_link invalidate];
    p->display_link = nil;
    p->display_callback = nil;
    [p->window orderOut:nil];
    [p->window setDelegate:nil];

    [p->window close];
    p->window = nil;

    [NSApp setDelegate:nil];
    free(p);
}

void *platform_get_swapchain(Platform *p) {
    return (__bridge void *)p->metal_layer;
}

vec2 platform_mouse_position(Platform *p) {
    // return mouse_point;
    NSPoint location = [NSEvent mouseLocation];
    NSPoint window_point = [p->window convertScreenToBase:location];
    NSPoint view_point = [p->view convertPoint:window_point fromView:nil];

    f32 view_height = p->view.frame.size.height;
    f32 y = view_height - view_point.y;

    return vec2_new(view_point.x, y);
}

vec2 platform_center_cursor() {
    // NSRect viewRect = [p->view frame];
    // NSPoint center = NSMakePoint(NSMidX(viewRect), NSMidY(viewRect));

    // NSPoint screen_point = [p->view convertPoint:center toView:nil];
    // screen_point = [p->window convertPointToScreen:screen_point];

    // CGAssociateMouseAndMouseCursorPosition(false);
    // CGWarpMouseCursorPosition(CGPointMake(screen_point.x, screen_point.y));
    // CGAssociateMouseAndMouseCursorPosition(true);

    // return vec2_new(center.x, center.y);
    return vec2_new(0, 0);
}
