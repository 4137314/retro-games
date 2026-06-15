use crossterm::{
    event::{self, DisableMouseCapture, EnableMouseCapture, Event, KeyCode},
    execute,
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
};
use ratatui::{
    backend::CrosstermBackend,
    layout::{Constraint, Direction as LayoutDirection, Layout},
    style::{Color, Modifier, Style},
    widgets::{Block, Borders, List, ListItem, ListState, Paragraph},
    Terminal, Frame,
};
use std::{error::Error, io, time::{Duration, Instant}};

mod games;
use games::{pong::PongGame, snake::SnakeGame, CurrentScreen};

struct App {
    current_screen: CurrentScreen,
    menu_options: Vec<&'static str>,
    menu_state: ListState,
    snake_game: SnakeGame,
    pong_game: PongGame,
}

impl App {
    fn new() -> Self {
        let mut menu_state = ListState::default();
        menu_state.select(Some(0));
        Self {
            current_screen: CurrentScreen::Menu,
            menu_options: vec!["🕹️  Gioca a Snake", "🏓  Gioca a Pong", "❌  Esci"],
            menu_state,
            snake_game: SnakeGame::new(),
            pong_game: PongGame::new(),
        }
    }

    fn menu_next(&mut self) {
        let i = match self.menu_state.selected() {
            Some(i) => {
                if i >= self.menu_options.len() - 1 { 0 } else { i + 1 }
            }
            None => 0,
        };
        self.menu_state.select(Some(i));
    }

    fn menu_prev(&mut self) {
        let i = match self.menu_state.selected() {
            Some(i) => {
                if i == 0 { self.menu_options.len() - 1 } else { i - 1 }
            }
            None => 0,
        };
        self.menu_state.select(Some(i));
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    // Setup terminale
    enable_raw_mode()?;
    let mut stdout = io::stdout();
    execute!(stdout, EnterAlternateScreen, EnableMouseCapture)?;
    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    let mut app = App::new();
    let res = run_app(&mut terminal, &mut app);

    // Ripristino terminale
    disable_raw_mode()?;
    execute!(
        terminal.backend_mut(),
        LeaveAlternateScreen,
        DisableMouseCapture
    )?;
    terminal.show_cursor()?;

    if let Err(err) = res {
        println!("Errore riscontrato: {:?}", err);
    }

    Ok(())
}

fn run_app<B: ratatui::backend::Backend>(
    terminal: &mut Terminal<B>,
    app: &mut App,
) -> Result<(), Box<dyn Error>> 
where 
    <B as ratatui::backend::Backend>::Error: 'static 
{
    let tick_rate = Duration::from_millis(100);
    let mut last_tick = Instant::now();

    loop {
        terminal.draw(|f| ui(f, app))?;

        let timeout = tick_rate
            .checked_sub(last_tick.elapsed())
            .unwrap_or_else(|| Duration::from_secs(0));

        if event::poll(timeout)? {
            if let Event::Key(key) = event::read()? {
                match app.current_screen {
                    CurrentScreen::Menu => match key.code {
                        KeyCode::Up | KeyCode::Char('w') => app.menu_prev(),
                        KeyCode::Down | KeyCode::Char('s') => app.menu_next(),
                        KeyCode::Enter => match app.menu_state.selected() {
                            Some(0) => app.current_screen = CurrentScreen::Snake,
                            Some(1) => app.current_screen = CurrentScreen::Pong,
                            _ => app.current_screen = CurrentScreen::Exiting,
                        },
                        KeyCode::Esc | KeyCode::Char('q') => app.current_screen = CurrentScreen::Exiting,
                        _ => {}
                    },
                    CurrentScreen::Snake => match key.code {
                        KeyCode::Esc | KeyCode::Char('q') => app.current_screen = CurrentScreen::Menu,
                        KeyCode::Up | KeyCode::Char('w') if app.snake_game.direction != games::snake::Direction::Down => {
                            app.snake_game.direction = games::snake::Direction::Up;
                        }
                        KeyCode::Down | KeyCode::Char('s') if app.snake_game.direction != games::snake::Direction::Up => {
                            app.snake_game.direction = games::snake::Direction::Down;
                        }
                        KeyCode::Left | KeyCode::Char('a') if app.snake_game.direction != games::snake::Direction::Right => {
                            app.snake_game.direction = games::snake::Direction::Left;
                        }
                        KeyCode::Right | KeyCode::Char('d') if app.snake_game.direction != games::snake::Direction::Left => {
                            app.snake_game.direction = games::snake::Direction::Right;
                        }
                        KeyCode::Char('r') if app.snake_game.game_over => {
                            app.snake_game = SnakeGame::new();
                        }
                        _ => {}
                    },
                    CurrentScreen::Pong => match key.code {
                        KeyCode::Esc | KeyCode::Char('q') => app.current_screen = CurrentScreen::Menu,
                        KeyCode::Up | KeyCode::Char('w') => app.pong_game.move_player(-1),
                        KeyCode::Down | KeyCode::Char('s') => app.pong_game.move_player(1),
                        _ => {}
                    }
                    _ => {}
                }
            }
        }

        if last_tick.elapsed() >= tick_rate {
            if app.current_screen == CurrentScreen::Snake {
                app.snake_game.update();
            } else if app.current_screen == CurrentScreen::Pong {
                app.pong_game.update();
            }
            last_tick = Instant::now();
        }

        if app.current_screen == CurrentScreen::Exiting {
            return Ok(());
        }
    }
}

fn ui(f: &mut Frame, app: &mut App) {
    let size = f.area();

    let chunks = Layout::default()
        .direction(LayoutDirection::Vertical)
        .constraints([Constraint::Length(3), Constraint::Min(0), Constraint::Length(3)])
        .split(size);

    let header = Paragraph::new("🕹️ ARCH RETRO-GAMES ARCADE 🕹️")
        .style(Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD))
        .block(Block::default().borders(Borders::BOTTOM))
        .alignment(ratatui::layout::Alignment::Center);
    f.render_widget(header, chunks[0]);

    match app.current_screen {
        CurrentScreen::Menu => {
            let items: Vec<ListItem> = app
                .menu_options
                .iter()
                .map(|i| ListItem::new(*i).style(Style::default().fg(Color::White)))
                .collect();

            let list = List::new(items)
                .block(Block::default().title(" Seleziona un gioco ").borders(Borders::ALL))
                .highlight_style(Style::default().bg(Color::Blue).add_modifier(Modifier::BOLD))
                .highlight_symbol(">> ");

            f.render_stateful_widget(list, chunks[1], &mut app.menu_state);
        }
        CurrentScreen::Snake => {
            app.snake_game.draw(f, chunks[1]);
        }
        CurrentScreen::Pong => {
            app.pong_game.draw(f, chunks[1]);
        }
        _ => {}
    }

    let footer_text = match app.current_screen {
        CurrentScreen::Menu => "Usa le Frecce/WASD per navigare, Invio per giocare, Esc/Q per uscire",
        CurrentScreen::Snake => "WASD/Frecce per sterzare | R per riavviare | Esc per tornare al menu",
        CurrentScreen::Pong => "W/S o Freccia Su/Giù per muovere la paletta | Esc per tornare al menu",
        _ => "",
    };
    let footer = Paragraph::new(footer_text)
        .style(Style::default().fg(Color::DarkGray))
        .block(Block::default().borders(Borders::TOP))
        .alignment(ratatui::layout::Alignment::Center);
    f.render_widget(footer, chunks[2]);
}
