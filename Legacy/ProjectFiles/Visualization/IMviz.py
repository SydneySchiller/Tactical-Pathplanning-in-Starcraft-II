
import pygame
 
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)
GREEN2 = (0, 153, 51)
 
WIDTH = 20
HEIGHT = 20
 
MARGIN = 5
 

grid = []
for row in range(10):

    grid.append([])
    for column in range(10):
        grid[row].append(0)  

pygame.init()
 
WINDOW_SIZE = [255, 255]
screen = pygame.display.set_mode(WINDOW_SIZE)
 
pygame.display.set_caption("Influence Map Example")
 
done = False
 
clock = pygame.time.Clock()
 
while not done:
    for event in pygame.event.get():  
        if event.type == pygame.QUIT: 
            done = True  
        elif event.type == pygame.MOUSEBUTTONDOWN:
            pos = pygame.mouse.get_pos()
            column = pos[0] // (WIDTH + MARGIN)
            row = pos[1] // (HEIGHT + MARGIN)
            grid[row][column] = 2
            grid[row+1][column] = 1 
            grid[row][column+1] = 1 
            grid[row-1][column] = 1
            grid[row][column-1] = 1
             
    screen.fill(BLACK)
 
    for row in range(10):
        for column in range(10):
            color = WHITE
            if grid[row][column] == 1:
                color = GREEN
            if grid[row][column] == 2:
                color = GREEN2
            pygame.draw.rect(screen,
                             color,
                             [(MARGIN + WIDTH) * column + MARGIN,
                              (MARGIN + HEIGHT) * row + MARGIN,
                              WIDTH,
                              HEIGHT])
    clock.tick(60)
    pygame.display.flip()
    
pygame.quit()
