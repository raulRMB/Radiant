import test from 'unit.js'
import MatchmakingQueue from "../../../src/clientAPI/matchmaking/matchmakingQueue.js"

describe('test 4 lobbies', () => {
  const queue = new MatchmakingQueue(3)

  const lobby1 = {
      openLobby: false,
      id: '1cd6332e-ec0b-4148-a895-c724a9468852',
      players: [
        { username: 'MASH', displayName: 'mash' },
        { username: 'MASH2', displayName: 'mash2' }
      ],
      leader: 'MASH'
  }
  
  const lobby2 = {
      openLobby: false,
      id: '2cd5332e-ec0b-4148-a8f5-c724a9468852',
      players: [
        { username: 'BASH', displayName: 'bash' },
        { username: 'BASH2', displayName: 'bash2' }
      ],
      leader: 'BASH'
  }
  
  const lobby3 = {
      openLobby: false,
      id: '3cd6332e-ec0b-4148-a895-c724a9468852',
      players: [
        { username: 'SMASH', displayName: 'smash' },
      ],
      leader: 'SMASH'
  }
  
  const lobby4 = {
      openLobby: false,
      id: '5ce6332e-ec0b-4148-a895-c724a9468852',
      players: [
        { username: 'DASH', displayName: 'dash' },
      ],
      leader: 'DASH'
  }
  queue.join(lobby1)
  queue.join(lobby2)
  queue.join(lobby3)
  queue.join(lobby4)
  const teams = queue.getTeams()
  it("should return 2 teams", () => {
    test
    .array(teams)
    .hasLength(2)
  })
  it("should have 6 players ready", () => {
    let players = 0
    teams.forEach(team => {
      team.lobbies.forEach(lobby => {
        players += lobby.players.length
      })
    })
    test.number(players).is(6)
  })
  it('lobby 1 and lobby 3 should have the same matchmaking id', () => {
    test.string(lobby1.matchmakingTicket).is(lobby3.matchmakingTicket)
  })
  it('lobby 2 and lobby 4 should have the same matchmaking id', () => {
    test.string(lobby2.matchmakingTicket).is(lobby4.matchmakingTicket)
  })
  it('lobby 1 and lobby 2 should have diff matchmaking id', () => {
    test.string(lobby1.matchmakingTicket).isNot(lobby2.matchmakingTicket)
  })
})

describe("Subqueue check basic join", () => {
  const x = new MatchmakingQueue(3)
  x.join({
    openLobby: false,
    id: '1cd6332e-ec0b-4148-a895-c724a9468852',
    players: [
      { username: 'MASH', displayName: 'mash' },
      { username: 'MASH2', displayName: 'mash2' }
    ],
    leader: 'MASH'
  })
  let subQueues = x.getSubQueues()
  it('should be 0 in the 1 person queue', () => {
    test.number(subQueues[0].length).is(0)
  })
  it('should be 1 in the 2 person queue', () => {
    test.number(subQueues[1].length).is(1)
  })
  it('should be 0 in the 3 person queue', () => {
    test.number(subQueues[2].length).is(0)
  })
})

describe("Subqueue check merging join", () => {
  const x = new MatchmakingQueue(3)
  const lobby1 = {
    openLobby: false,
    id: '3cd6332e-ec0b-4148-a895-c724a9468852',
    players: [
      { username: 'SMASH', displayName: 'smash' },
    ],
    leader: 'SMASH'
  }
  const lobby2 = {
    openLobby: false,
    id: '1cd6332e-ec0b-4148-a895-c724a9468852',
    players: [
      { username: 'MASH', displayName: 'mash' },
      { username: 'MASH2', displayName: 'mash2' }
    ],
    leader: 'MASH'
  }
  x.join(lobby1)
  x.join(lobby2)
  let subQueues = x.getSubQueues()
  it('should be 0 in the 1 person queue', () => {
    test.number(subQueues[0].length).is(0)
  })
  it('should be 0 in the 2 person queue', () => {
    test.number(subQueues[1].length).is(0)
  })
  it('should be 1 in the 3 person queue', () => {
    test.number(subQueues[2].length).is(1)
  })
  it('should have the same matchmaking id', () => {
    test.string(lobby1.matchmakingTicket).is(lobby2.matchmakingTicket)
  })
})

// describe("Test matchmaking queue with not enough players for game", () => {
  
//   queue.join(lobby1)
//   queue.join(lobby2)
//   queue.join(lobby3)
//   const teams = queue.getTeams()
//   it("should return null", () => {
//     test.value(teams).isNull()
//   })

// })