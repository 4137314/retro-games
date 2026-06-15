use ratatui::{
    layout::Rect,
    style::{Color, Style},
    widgets::{Block, Borders, Paragraph},
    Frame,
};
use std::collections::VecDeque;

#[derive(Debug, Copy, Clone, PartialEq)]
pub enum Direction { Up, Down, Left, Right }

pub struct SnakeGame {
    pub snake: VecDeque<(u16, u16)>,
    pub direction: Direction,
    pub food: (u16, u16),
    pub score: u32,
    pub game_over: bool,
    width: u16,
    height: u16,
}

impl SnakeGame {
    pub fn new() -> Self {
        let mut snake = VecDeque::new();
        snake.push_back((10, 5));
        snake.push_back((9, 5));
        
        Self {
            snake,
            direction: Direction::Right,
            food: (15, 7),
            score: 0,
            game_over: false,
            width: 40,
            height: 20,
        }
    }

    pub fn update(&mut self) {
        if self.game_over { return; }

        let head = self.snake.front().unwrap();
        let new_head = match self.direction {
            Direction::Up => (head.0, head.1.saturating_sub(1)),
            Direction::Down => (head.0, head.1 + 1),
            Direction::Left => (head.0.saturating_sub(1), head.1),
            Direction::Right => (head.0 + 1, head.1),
        };

        // Gestione bordi (retro-style: muore se tocca il muro)
        if new_head.0 == 0 || new_head.0 >= self.width - 1 || new_head.1 == 0 || new_head.1 >= self.height - 1 {
            self.game_over = true;
            return;
        }

        // Auto-collisione
        if self.snake.contains(&new_head) {
            self.game_over = true;
            return;
        }

        self.snake.push_front(new_head);

        // Controllo se mangia il cibo
        if new_head == self.food {
            self.score += 10;
            self.spawn_food();
        } else {
            self.snake.pop_back();
        }
    }

    fn spawn_food(&mut self) {
        // Logica finta-random semplice per non appesantire con la crate 'rand'
        let next_x = ((self.food.0 * 7 + 3) % (self.width - 2)) + 1;
        let next_y = ((self.food.1 * 11 + 5) % (self.height - 2)) + 1;
        self.food = (next_x, next_y);
    }

    pub fn draw(&self, frame: &mut Frame, area: Rect) {
        let game_block = Block::default()
            .title(format!(" 🐍 SNAKE - SCORE: {} ", self.score))
            .borders(Borders::ALL)
            .style(Style::default().fg(Color::Green));

        frame.render_widget(game_block, area);

        // Disegna il cibo
        let food_rect = Rect::new(area.x + self.food.0, area.y + self.food.1, 1, 1);
        frame.render_widget(Paragraph::new("🍎"), food_rect);

        // Disegna il serpente
        for (x, y) in &self.snake {
            let body_rect = Rect::new(area.x + *x, area.y + *y, 1, 1);
            frame.render_widget(Paragraph::new("🟩"), body_rect);
        }

        if self.game_over {
            let go_rect = Rect::new(area.x + (self.width / 4), area.y + (self.height / 2), 20, 3);
            frame.render_widget(Paragraph::new(" GAME OVER!\n Premi 'R' per resettare ").block(Block::default().borders(Borders::ALL)), go_rect);
        }
    }
}
