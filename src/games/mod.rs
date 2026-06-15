pub mod snake;
pub mod pong;

#[derive(PartialEq, Eq)]
pub enum CurrentScreen {
    Menu,
    Snake,
    Pong,
    Exiting,
}

pub enum MenuSelection {
    Snake,
    Pong,
    Exit,
}
