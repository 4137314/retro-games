use ratatui::{
    layout::Rect,
    style::{Color, Style},
    widgets::{Block, Borders, Paragraph},
    Frame,
};

pub struct PongGame {
    pub ball_x: i16,
    pub ball_y: i16,
    pub ball_dx: i16,
    pub ball_dy: i16,
    pub player_y: i16,
    pub ai_y: i16,
    pub player_score: u32,
    pub ai_score: u32,
    width: i16,
    height: i16,
}

impl PongGame {
    pub fn new() -> Self {
        Self {
            ball_x: 20,
            ball_y: 10,
            ball_dx: 1,
            ball_dy: 1,
            player_y: 8,
            ai_y: 8,
            player_score: 0,
            ai_score: 0,
            width: 50,
            height: 18,
        }
    }

    pub fn update(&mut self) {
        // Muovi palla
        self.ball_x += self.ball_dx;
        self.ball_y += self.ball_dy;

        // Rimbalzo sopra/sotto
        if self.ball_y <= 1 || self.ball_y >= self.height - 2 {
            self.ball_dy = -self.ball_dy;
        }

        // Movimento IA (insegue la palla in modo pigro)
        if self.ball_y > self.ai_y + 1 && self.ai_y < self.height - 4 {
            self.ai_y += 1;
        } else if self.ball_y < self.ai_y && self.ai_y > 1 {
            self.ai_y -= 1;
        }

        // Collisione Paletta Giocatore (Sinistra, x=2)
        if self.ball_x == 2 && self.ball_y >= self.player_y && self.ball_y <= self.player_y + 3 {
            self.ball_dx = -self.ball_dx;
        }

        // Collisione Paletta IA (Destra, x=width-3)
        if self.ball_x == self.width - 3 && self.ball_y >= self.ai_y && self.ball_y <= self.ai_y + 3 {
            self.ball_dx = -self.ball_dx;
        }

        // Punti
        if self.ball_x <= 0 {
            self.ai_score += 1;
            self.reset_ball();
        } else if self.ball_x >= self.width {
            self.player_score += 1;
            self.reset_ball();
        }
    }

    fn reset_ball(&mut self) {
        self.ball_x = self.width / 2;
        self.ball_y = self.height / 2;
        self.ball_dx = -self.ball_dx;
    }

    pub fn move_player(&mut self, delta: i16) {
        let next_y = self.player_y + delta;
        if next_y >= 1 && next_y <= self.height - 4 {
            self.player_y = next_y;
        }
    }

    pub fn draw(&self, frame: &mut Frame, area: Rect) {
        let title = format!(" 🏓 PONG | Tu: {} - IA: {} ", self.player_score, self.ai_score);
        let game_block = Block::default()
            .title(title)
            .borders(Borders::ALL)
            .style(Style::default().fg(Color::Cyan));

        frame.render_widget(game_block, area);

        // Disegna palla
        let ball_rect = Rect::new(area.x + self.ball_x as u16, area.y + self.ball_y as u16, 1, 1);
        frame.render_widget(Paragraph::new("⚪"), ball_rect);

        // Disegna Paletta Giocatore (Sinistra)
        for i in 0..3 {
            let p_rect = Rect::new(area.x + 2, area.y + (self.player_y + i) as u16, 1, 1);
            frame.render_widget(Paragraph::new("█"), p_rect);
        }

        // Disegna Paletta IA (Destra)
        for i in 0..3 {
            let ai_rect = Rect::new(area.x + (self.width - 3) as u16, area.y + (self.ai_y + i) as u16, 1, 1);
            frame.render_widget(Paragraph::new("█"), ai_rect);
        }
    }
}
